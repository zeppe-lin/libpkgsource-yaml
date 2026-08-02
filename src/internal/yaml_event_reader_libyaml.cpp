// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "yaml_event_reader.h"

#include <libpkgsource-yaml/parser.h>

#include <yaml.h>

#include <limits>
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

  throw yaml_error(yaml_error_code::syntax, {}, "$", 1, 1,
                   "libyaml returned an unknown event type");
}

} // namespace

class yaml_event_reader::implementation final {
public:
  implementation(std::string_view bytes, std::string document,
                 std::size_t maximum_scalar_bytes)
      : document_(std::move(document)),
        maximum_scalar_bytes_(maximum_scalar_bytes)
  {
    if (yaml_parser_initialize(&parser_) == 0) {
      throw yaml_error(yaml_error_code::syntax, document_, "$", 1, 1,
                       "cannot initialize YAML parser");
    }
    initialized_ = true;

    static constexpr unsigned char empty_input = 0;
    const auto* input = bytes.empty()
        ? &empty_input
        : reinterpret_cast<const unsigned char*>(bytes.data());
    yaml_parser_set_input_string(&parser_, input, bytes.size());
  }

  ~implementation()
  {
    if (initialized_) {
      yaml_parser_delete(&parser_);
    }
  }

  [[nodiscard]] yaml_event next()
  {
    yaml_event_t native_event{};
    if (yaml_parser_parse(&parser_, &native_event) == 0) {
      const source_mark mark{
          one_based_position(parser_.problem_mark.line),
          one_based_position(parser_.problem_mark.column),
      };
      const std::string problem = parser_.problem != nullptr
          ? parser_.problem
          : "invalid YAML input";
      throw yaml_error(yaml_error_code::syntax, document_, "$", mark.line,
                       mark.column, problem);
    }

    yaml_event result;
    result.kind = event_kind(native_event.type);
    result.mark = {
        one_based_position(native_event.start_mark.line),
        one_based_position(native_event.start_mark.column),
    };

    switch (native_event.type) {
      case YAML_DOCUMENT_START_EVENT:
        result.has_directives =
            native_event.data.document_start.version_directive != nullptr
            || native_event.data.document_start.tag_directives.start
                != native_event.data.document_start.tag_directives.end;
        break;
      case YAML_ALIAS_EVENT:
        result.anchor = copy_yaml_string(native_event.data.alias.anchor);
        break;
      case YAML_SCALAR_EVENT:
        if (native_event.data.scalar.length > maximum_scalar_bytes_) {
          yaml_event_delete(&native_event);
          throw yaml_error(
              yaml_error_code::resource_limit, document_, "$",
              result.mark.line, result.mark.column,
              "YAML scalar exceeds configured byte limit");
        }
        result.value.assign(
            reinterpret_cast<const char*>(native_event.data.scalar.value),
            native_event.data.scalar.length);
        result.anchor = copy_yaml_string(native_event.data.scalar.anchor);
        result.tag = copy_yaml_string(native_event.data.scalar.tag);
        break;
      case YAML_SEQUENCE_START_EVENT:
        result.anchor =
            copy_yaml_string(native_event.data.sequence_start.anchor);
        result.tag = copy_yaml_string(native_event.data.sequence_start.tag);
        break;
      case YAML_MAPPING_START_EVENT:
        result.anchor =
            copy_yaml_string(native_event.data.mapping_start.anchor);
        result.tag = copy_yaml_string(native_event.data.mapping_start.tag);
        break;
      default:
        break;
    }

    yaml_event_delete(&native_event);
    return result;
  }

private:
  yaml_parser_t parser_{};
  bool initialized_ = false;
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
