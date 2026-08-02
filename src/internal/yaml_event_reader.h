// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "source_mark.h"

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>

namespace pkgsource::yaml::internal {

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

struct yaml_event final {
  yaml_event_kind kind;
  source_mark mark{1, 1};
  std::string value;
  std::string anchor;
  std::string tag;
  bool has_directives = false;
};

class yaml_event_reader final {
public:
  yaml_event_reader(std::string_view bytes, std::string document,
                    std::size_t maximum_scalar_bytes);
  ~yaml_event_reader();

  yaml_event_reader(const yaml_event_reader&) = delete;
  yaml_event_reader& operator=(const yaml_event_reader&) = delete;
  yaml_event_reader(yaml_event_reader&&) = delete;
  yaml_event_reader& operator=(yaml_event_reader&&) = delete;

  [[nodiscard]] yaml_event next();

private:
  class implementation;
  std::unique_ptr<implementation> implementation_;
};

} // namespace pkgsource::yaml::internal
