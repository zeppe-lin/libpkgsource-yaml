// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "source_mark.h"

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>

namespace pkgsource::yaml::internal {

/// Provider-neutral event kinds consumed by the document-tree parser.
enum class yaml_event_kind {
  stream_start,
  stream_end,
  document_start,
  document_end,
  alias,
  scalar,
  sequence_start,
  sequence_end,
  mapping_start,
  mapping_end,
};

/// One normalized YAML provider event with copied caller-visible material.
struct yaml_event final {
  yaml_event_kind kind;
  source_mark mark{1, 1};
  std::string value;
  std::string anchor;
  std::string tag;
  bool has_directives = false;
};

/**
 * Pull normalized events from one caller-owned in-memory YAML stream.
 *
 * Provider-native parser and event representations remain hidden behind the
 * implementation pointer. Returned strings own their bytes and remain valid
 * after the next event is read.
 */
class yaml_event_reader final {
public:
  yaml_event_reader(std::string_view bytes,
                    std::string document,
                    std::size_t maximum_scalar_bytes);
  ~yaml_event_reader();

  yaml_event_reader(const yaml_event_reader&) = delete;
  yaml_event_reader& operator=(const yaml_event_reader&) = delete;
  yaml_event_reader(yaml_event_reader&&) = delete;
  yaml_event_reader& operator=(yaml_event_reader&&) = delete;

  /// Return the next normalized event or throw yaml_error on provider failure.
  [[nodiscard]] yaml_event next();

private:
  class implementation;
  std::unique_ptr<implementation> implementation_;
};

} // namespace pkgsource::yaml::internal
