// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "internal/document.h"

#include <string>
#include <utility>
#include <vector>

namespace pkgsource::yaml {

std::vector<profile_declaration> parse_profiles_yaml(
    std::string_view bytes, source_origin origin,
    const parse_limits& limits)
{
  using namespace internal;
  node root = parse_document(bytes, origin, limits);
  allow_keys(root, origin, "$", {"format", "profiles"});
  require_format(root, origin, "zeppe-lin.profiles/1");
  const node& profiles = required_key(root, "profiles", origin, "$");
  require_kind(profiles, node_kind::mapping, origin, "profiles", "profiles");

  std::vector<profile_declaration> declarations;
  declarations.reserve(profiles.children.size() / 2);
  for (std::size_t i = 0; i < profiles.children.size(); i += 2) {
    const node& name_node = profiles.children[i];
    const node& definition = profiles.children[i + 1];
    const std::string definition_path = "profiles." + name_node.scalar;
    allow_keys(definition, origin, definition_path, {"members"});
    const node& members_node = required_key(
        definition, "members", origin, definition_path);
    const auto& member_nodes = sequence_value(
        members_node, origin, definition_path + ".members");
    std::vector<profile_member_declaration> members;
    members.reserve(member_nodes.size());
    for (std::size_t member_index = 0;
         member_index < member_nodes.size(); ++member_index) {
      const std::string member_path = definition_path + ".members["
          + std::to_string(member_index) + "]";
      requirement_subject subject = subject_value(
          member_nodes[member_index], origin, member_path);
      members.emplace_back(
          std::move(subject),
          provenance(origin, member_path, member_nodes[member_index]));
    }
    declarations.push_back(semantic_value(
        origin, definition_path, name_node, [&] {
          return profile_declaration(
              profile_reference("@" + name_node.scalar),
              provenance(origin, definition_path, name_node),
              std::move(members));
        }));
  }
  return declarations;
}

} // namespace pkgsource::yaml
