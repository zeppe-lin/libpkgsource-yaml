// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include "document.h"

#include "yaml_event_reader.h"

#include <algorithm>
#include <set>
#include <string>
#include <utility>

namespace pkgsource::yaml::internal {
namespace {

struct parse_state final {
  const parse_limits& limits;
  std::size_t nodes = 0;
};

bool permitted_tag(node_kind kind, std::string_view tag)
{
  if (tag.empty()) {
    return true;
  }

  // Explicit standard tags are accepted only for the node kind they name.
  // Scalar tag resolution never creates package semantics; scalar bytes remain
  // strings until the grammar invokes the corresponding owner constructor.
  switch (kind) {
  case node_kind::scalar:
    return tag == "tag:yaml.org,2002:str" || tag == "tag:yaml.org,2002:int";
  case node_kind::sequence:
    return tag == "tag:yaml.org,2002:seq";
  case node_kind::mapping:
    return tag == "tag:yaml.org,2002:map";
  }

  return false;
}

void account_node(parse_state& state,
                  const source_origin& origin,
                  const std::string& path,
                  source_mark mark,
                  std::size_t depth)
{
  if (depth > state.limits.maximum_depth) {
    fail(yaml_error_code::resource_limit,
         origin,
         path,
         mark,
         "YAML nesting exceeds configured depth limit");
  }
  if (state.nodes >= state.limits.maximum_nodes) {
    fail(yaml_error_code::resource_limit,
         origin,
         path,
         mark,
         "YAML node count exceeds configured limit");
  }
  ++state.nodes;
}

node parse_node(yaml_event_reader& reader,
                yaml_event first,
                const source_origin& origin,
                const std::string& path,
                parse_state& state,
                std::size_t depth)
{
  account_node(state, origin, path, first.mark, depth);

  if (first.kind == yaml_event_kind::alias) {
    fail(yaml_error_code::unsupported_feature,
         origin,
         path,
         first.mark,
         "YAML aliases are not supported");
  }

  node_kind kind;
  switch (first.kind) {
  case yaml_event_kind::scalar:
    kind = node_kind::scalar;
    break;
  case yaml_event_kind::sequence_start:
    kind = node_kind::sequence;
    break;
  case yaml_event_kind::mapping_start:
    kind = node_kind::mapping;
    break;
  default:
    fail(yaml_error_code::invalid_document,
         origin,
         path,
         first.mark,
         "expected a YAML value");
  }

  if (!first.anchor.empty()) {
    fail(yaml_error_code::unsupported_feature,
         origin,
         path,
         first.mark,
         "YAML anchors are not supported");
  }
  if (!permitted_tag(kind, first.tag)) {
    fail(yaml_error_code::unsupported_feature,
         origin,
         path,
         first.mark,
         "custom or incompatible YAML tags are not supported");
  }

  node result{kind, first.mark, {}, {}};
  if (kind == node_kind::scalar) {
    result.scalar = std::move(first.value);
    return result;
  }

  std::size_t index = 0;
  while (true) {
    yaml_event next = reader.next();
    const bool sequence_end = kind == node_kind::sequence &&
                              next.kind == yaml_event_kind::sequence_end;
    const bool mapping_end =
        kind == node_kind::mapping && next.kind == yaml_event_kind::mapping_end;
    if (sequence_end || mapping_end) {
      break;
    }
    if (next.kind == yaml_event_kind::stream_end ||
        next.kind == yaml_event_kind::document_end) {
      fail(yaml_error_code::syntax,
           origin,
           path,
           next.mark,
           "unterminated YAML collection");
    }

    const std::string item_path = kind == node_kind::sequence
                                      ? path + "[" + std::to_string(index) + "]"
                                      : path;
    result.children.push_back(parse_node(
        reader, std::move(next), origin, item_path, state, depth + 1));
    ++index;
  }

  if (kind == node_kind::mapping && result.children.size() % 2 != 0) {
    fail(yaml_error_code::syntax,
         origin,
         path,
         result.mark,
         "mapping has an incomplete key/value pair");
  }
  return result;
}

void validate_tree(const node& value,
                   const source_origin& origin,
                   const std::string& path)
{
  if (value.kind == node_kind::sequence) {
    for (std::size_t i = 0; i < value.children.size(); ++i) {
      validate_tree(
          value.children[i], origin, path + "[" + std::to_string(i) + "]");
    }
    return;
  }
  if (value.kind != node_kind::mapping) {
    return;
  }

  std::set<std::string> keys;
  for (std::size_t i = 0; i < value.children.size(); i += 2) {
    const node& key = value.children[i];
    const node& child = value.children[i + 1];
    if (key.kind != node_kind::scalar) {
      fail(yaml_error_code::invalid_type,
           origin,
           path,
           key.mark,
           "mapping keys must be scalars");
    }
    if (key.scalar == "<<") {
      fail(yaml_error_code::unsupported_feature,
           origin,
           path,
           key.mark,
           "YAML merge keys are not supported");
    }
    if (!keys.insert(key.scalar).second) {
      fail(yaml_error_code::duplicate_key,
           origin,
           child_path(path, key.scalar),
           key.mark,
           "duplicate mapping key: " + key.scalar);
    }
    validate_tree(child, origin, child_path(path, key.scalar));
  }
}

} // namespace

[[noreturn]] void fail(yaml_error_code code,
                       const source_origin& origin,
                       std::string_view path,
                       source_mark mark,
                       std::string message)
{
  throw yaml_error(code,
                   origin.document(),
                   std::string(path),
                   mark.line,
                   mark.column,
                   std::move(message));
}

node parse_document(std::string_view bytes,
                    const source_origin& origin,
                    const parse_limits& limits)
{
  if (bytes.size() > limits.maximum_document_bytes) {
    fail(yaml_error_code::resource_limit,
         origin,
         "$",
         {1, 1},
         "YAML document exceeds configured byte limit");
  }

  yaml_event_reader reader(
      bytes, origin.document(), limits.maximum_scalar_bytes);
  yaml_event event = reader.next();
  if (event.kind != yaml_event_kind::stream_start) {
    fail(yaml_error_code::syntax,
         origin,
         "$",
         event.mark,
         "missing YAML stream start");
  }

  event = reader.next();
  if (event.kind != yaml_event_kind::document_start) {
    fail(yaml_error_code::invalid_document,
         origin,
         "$",
         event.mark,
         "expected one YAML document");
  }
  if (event.has_directives) {
    fail(yaml_error_code::unsupported_feature,
         origin,
         "$",
         event.mark,
         "YAML directives are not supported");
  }

  event = reader.next();
  if (event.kind == yaml_event_kind::document_end) {
    fail(yaml_error_code::invalid_document,
         origin,
         "$",
         event.mark,
         "empty YAML document");
  }

  parse_state state{limits, 0};
  node root = parse_node(reader, std::move(event), origin, "$", state, 1);

  event = reader.next();
  if (event.kind != yaml_event_kind::document_end) {
    fail(yaml_error_code::syntax,
         origin,
         "$",
         event.mark,
         "expected YAML document end");
  }
  event = reader.next();
  if (event.kind == yaml_event_kind::document_start) {
    fail(yaml_error_code::invalid_document,
         origin,
         "$",
         event.mark,
         "multiple YAML documents are not supported");
  }
  if (event.kind != yaml_event_kind::stream_end) {
    fail(yaml_error_code::syntax,
         origin,
         "$",
         event.mark,
         "expected YAML stream end");
  }

  // Mapping paths and duplicate-key checks require complete key/value pairs.
  // Perform that validation after provider events have produced the bounded
  // tree and before any document grammar consumes it.
  validate_tree(root, origin, "$");
  return root;
}

std::string child_path(std::string_view path, std::string_view key)
{
  if (path == "$") {
    return std::string(key);
  }
  return std::string(path) + "." + std::string(key);
}

const node& require_kind(const node& value,
                         node_kind expected,
                         const source_origin& origin,
                         std::string_view path,
                         std::string_view name)
{
  if (value.kind != expected) {
    fail(yaml_error_code::invalid_type,
         origin,
         path,
         value.mark,
         std::string(name) + " has the wrong YAML type");
  }
  return value;
}

const node* find_key(const node& mapping, std::string_view key)
{
  for (std::size_t i = 0; i < mapping.children.size(); i += 2) {
    if (mapping.children[i].scalar == key) {
      return &mapping.children[i + 1];
    }
  }
  return nullptr;
}

const node& required_key(const node& mapping,
                         std::string_view key,
                         const source_origin& origin,
                         std::string_view path)
{
  if (const node* value = find_key(mapping, key)) {
    return *value;
  }
  fail(yaml_error_code::missing_key,
       origin,
       child_path(path, key),
       mapping.mark,
       "missing required key: " + std::string(key));
}

void allow_keys(const node& mapping,
                const source_origin& origin,
                std::string_view path,
                std::initializer_list<std::string_view> allowed)
{
  require_kind(mapping, node_kind::mapping, origin, path, path);
  for (std::size_t i = 0; i < mapping.children.size(); i += 2) {
    const node& key = mapping.children[i];
    if (std::find(allowed.begin(), allowed.end(), key.scalar) ==
        allowed.end()) {
      fail(yaml_error_code::unknown_key,
           origin,
           child_path(path, key.scalar),
           key.mark,
           "unknown key: " + key.scalar);
    }
  }
}

const std::string& scalar_value(const node& value,
                                const source_origin& origin,
                                std::string_view path)
{
  require_kind(value, node_kind::scalar, origin, path, path);
  return value.scalar;
}

const std::vector<node>& sequence_value(const node& value,
                                        const source_origin& origin,
                                        std::string_view path)
{
  require_kind(value, node_kind::sequence, origin, path, path);
  return value.children;
}

declaration_provenance
provenance(const source_origin& origin, std::string path, const node& value)
{
  return declaration_provenance(
      origin.document(), std::move(path), value.mark.line, value.mark.column);
}

requirement_subject subject_value(const node& value,
                                  const source_origin& origin,
                                  const std::string& path)
{
  allow_keys(value, origin, path, {"package", "profile"});
  const node* package = find_key(value, "package");
  const node* profile = find_key(value, "profile");
  if ((package != nullptr) == (profile != nullptr)) {
    fail(yaml_error_code::invalid_value,
         origin,
         path,
         value.mark,
         "requirement subject must contain exactly one of package or profile");
  }
  if (package != nullptr) {
    const std::string package_path = child_path(path, "package");
    const std::string name = scalar_value(*package, origin, package_path);
    return semantic_value(origin, package_path, *package, [&] {
      return requirement_subject(package_reference(name));
    });
  }

  const std::string profile_path = child_path(path, "profile");
  const std::string name = scalar_value(*profile, origin, profile_path);
  return semantic_value(origin, profile_path, *profile, [&] {
    return requirement_subject(profile_reference(name));
  });
}

void require_format(const node& root,
                    const source_origin& origin,
                    std::string_view expected)
{
  const node& format = required_key(root, "format", origin, "$");
  if (scalar_value(format, origin, "format") != expected) {
    fail(yaml_error_code::invalid_value,
         origin,
         "format",
         format.mark,
         "unsupported document format; expected " + std::string(expected));
  }
}

} // namespace pkgsource::yaml::internal
