// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgsource-yaml/parser.h>

#include "yaml_event_reader.h"

#include <yaml.h>

#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <utility>

namespace pkgsource::yaml::internal {
namespace {

std::uint32_t one_based_position(std::size_t value)
{
  if (value >= std::numeric_limits<std::uint32_t>::max()) {
    return std::numeric_limits<std::uint32_t>::max();
  }
  return static_cast<std::uint32_t>(value + 1);
}

std::string copy_yaml_string(const yaml_char_t* value)
{
  if (value == nullptr) {
    return {};
  }
  return reinterpret_cast<const char*>(value);
}

yaml_event_kind event_kind(yaml_event_type_t type)
{
  switch (type) {
  case YAML_STREAM_START_EVENT:
    return yaml_event_kind::stream_start;
  case YAML_STREAM_END_EVENT:
    return yaml_event_kind::stream_end;
  case YAML_DOCUMENT_START_EVENT:
    return yaml_event_kind::document_start;
  case YAML_DOCUMENT_END_EVENT:
    return yaml_event_kind::document_end;
  case YAML_ALIAS_EVENT:
    return yaml_event_kind::alias;
  case YAML_SCALAR_EVENT:
    return yaml_event_kind::scalar;
  case YAML_SEQUENCE_START_EVENT:
    return yaml_event_kind::sequence_start;
  case YAML_SEQUENCE_END_EVENT:
    return yaml_event_kind::sequence_end;
  case YAML_MAPPING_START_EVENT:
    return yaml_event_kind::mapping_start;
  case YAML_MAPPING_END_EVENT:
    return yaml_event_kind::mapping_end;
  case YAML_NO_EVENT:
    break;
  }

  // A successful yaml_parser_parse() call must produce one documented event.
  // Treat an unknown provider value as an internal provider defect rather than
  // misreporting it as malformed caller input.
  throw std::logic_error("libyaml returned an unknown event type");
}

// Own one libyaml event across every C++ exit path. String allocation and
// internal validation can throw after yaml_parser_parse() succeeds.
class native_event final {
public:
  native_event() = default;

  ~native_event()
  {
    if (initialized_) {
      yaml_event_delete(&value_);
    }
  }

  native_event(const native_event&) = delete;
  native_event& operator=(const native_event&) = delete;

  [[nodiscard]] yaml_event_t* output() noexcept
  {
    return &value_;
  }

  void mark_initialized() noexcept
  {
    initialized_ = true;
  }

  [[nodiscard]] const yaml_event_t& value() const noexcept
  {
    return value_;
  }

private:
  yaml_event_t value_{};
  bool initialized_ = false;
};

} // namespace

class yaml_event_reader::implementation final {
public:
  implementation(std::string_view bytes,
                 std::string document,
                 std::size_t maximum_scalar_bytes)
      : document_(std::move(document)),
        maximum_scalar_bytes_(maximum_scalar_bytes)
  {
    if (yaml_parser_initialize(&parser_) == 0) {
      throw std::bad_alloc();
    }

    static constexpr unsigned char empty_input = 0;
    const auto* input =
        bytes.empty() ? &empty_input
                      : reinterpret_cast<const unsigned char*>(bytes.data());
    yaml_parser_set_input_string(&parser_, input, bytes.size());
  }

  ~implementation()
  {
    yaml_parser_delete(&parser_);
  }

  [[nodiscard]] yaml_event next()
  {
    native_event event;
    if (yaml_parser_parse(&parser_, event.output()) == 0) {
      switch (parser_.error) {
      case YAML_MEMORY_ERROR:
        throw std::bad_alloc();
      case YAML_READER_ERROR:
      case YAML_SCANNER_ERROR:
      case YAML_PARSER_ERROR:
        break;
      case YAML_NO_ERROR:
      case YAML_COMPOSER_ERROR:
      case YAML_WRITER_ERROR:
      case YAML_EMITTER_ERROR:
        throw std::logic_error(
            "libyaml parser failed outside parser error domain");
      }

      const source_mark mark{
          one_based_position(parser_.problem_mark.line),
          one_based_position(parser_.problem_mark.column),
      };
      const std::string problem =
          parser_.problem != nullptr ? parser_.problem : "invalid YAML input";
      throw yaml_error(yaml_error_code::syntax,
                       document_,
                       "$",
                       mark.line,
                       mark.column,
                       problem);
    }
    event.mark_initialized();

    const yaml_event_t& native = event.value();
    yaml_event result;
    result.kind = event_kind(native.type);
    result.mark = {
        one_based_position(native.start_mark.line),
        one_based_position(native.start_mark.column),
    };

    switch (native.type) {
    case YAML_DOCUMENT_START_EVENT:
      result.has_directives =
          native.data.document_start.version_directive != nullptr ||
          native.data.document_start.tag_directives.start !=
              native.data.document_start.tag_directives.end;
      break;
    case YAML_ALIAS_EVENT:
      result.anchor = copy_yaml_string(native.data.alias.anchor);
      break;
    case YAML_SCALAR_EVENT:
      if (native.data.scalar.length > maximum_scalar_bytes_) {
        throw yaml_error(yaml_error_code::resource_limit,
                         document_,
                         "$",
                         result.mark.line,
                         result.mark.column,
                         "YAML scalar exceeds configured byte limit");
      }
      result.value.assign(
          reinterpret_cast<const char*>(native.data.scalar.value),
          native.data.scalar.length);
      result.anchor = copy_yaml_string(native.data.scalar.anchor);
      result.tag = copy_yaml_string(native.data.scalar.tag);
      break;
    case YAML_SEQUENCE_START_EVENT:
      result.anchor = copy_yaml_string(native.data.sequence_start.anchor);
      result.tag = copy_yaml_string(native.data.sequence_start.tag);
      break;
    case YAML_MAPPING_START_EVENT:
      result.anchor = copy_yaml_string(native.data.mapping_start.anchor);
      result.tag = copy_yaml_string(native.data.mapping_start.tag);
      break;
    default:
      break;
    }

    return result;
  }

private:
  yaml_parser_t parser_{};
  std::string document_;
  std::size_t maximum_scalar_bytes_;
};

yaml_event_reader::yaml_event_reader(std::string_view bytes,
                                     std::string document,
                                     std::size_t maximum_scalar_bytes)
    : implementation_(std::make_unique<implementation>(
          bytes, std::move(document), maximum_scalar_bytes))
{
}

yaml_event_reader::~yaml_event_reader() = default;

yaml_event yaml_event_reader::next()
{
  return implementation_->next();
}

} // namespace pkgsource::yaml::internal
