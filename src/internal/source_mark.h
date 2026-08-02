// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>

namespace pkgsource::yaml::internal {

struct source_mark final {
  std::uint32_t line;
  std::uint32_t column;
};

} // namespace pkgsource::yaml::internal
