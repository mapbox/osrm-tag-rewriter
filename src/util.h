#pragma once

#include <cstdlib>
#include <cstring>

#include <string>
#include <utility>

#include <osmium/builder/osm_object_builder.hpp>
#include <osmium/osm/types.hpp>
#include <osmium/tags/taglist.hpp>

inline bool Equal(const char *lhs, const char *rhs) { return std::strcmp(lhs, rhs) == 0; }

struct Committer {
  osmium::memory::Buffer &buffer;
  ~Committer() noexcept { buffer.commit(); }
};

template <typename Builder> //
inline void Copy(Builder &builder, const osmium::OSMObject &object) {
  builder.set_id(object.id())
      .set_version(object.version())
      .set_changeset(object.changeset())
      .set_timestamp(object.timestamp())
      .set_uid(object.uid())
      .set_user(object.user());
}

inline void Extend(osmium::builder::Builder &parent, const osmium::TagList &tags, const char *key, const char *value) {
  osmium::builder::TagListBuilder builder{parent};

  for (const auto &tag : tags)
    builder.add_tag(tag);

  builder.add_tag(key, value);
}
