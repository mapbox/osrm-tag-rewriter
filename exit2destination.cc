#include <cstdlib>
#include <cstring>

#include <iterator>
#include <string>
#include <unordered_map>
#include <utility>

#include <osmium/builder/osm_object_builder.hpp>
#include <osmium/handler.hpp>
#include <osmium/io/pbf_input.hpp>
#include <osmium/io/pbf_output.hpp>
#include <osmium/io/reader.hpp>
#include <osmium/io/writer.hpp>
#include <osmium/osm/types.hpp>
#include <osmium/visitor.hpp>

// Ideas for Improvement:
//  - In the way() function exit early when not highway
//  - Inject output buffer size hint based on input buffer .committed() + expected destination tag length
//  - Handle ref and other tags on the motorway junction node in the same way (check first if worth it)
//  - Combine with planet pre-filtering: think filtering relations other than turn restrictions, buildings, trees

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

// Collects node ids and exit_to=* tags on highway=motorway_junction.
// http://wiki.openstreetmap.org/wiki/Key:exit_to
// http://taginfo.openstreetmap.org/keys/exit_to
struct ExitToRewriter : osmium::handler::Handler {
  static const constexpr auto kExitToTagsInPlanet = 35000u;
  static const constexpr auto kBufferCapacityHint = 1u << 12u;

  ExitToRewriter() { destinations.reserve(kExitToTagsInPlanet); }

  void node(const osmium::Node &node) {
    Committer defer{outbuf};

    // Copy over node data untouched
    outbuf.add_item(node);

    // Record exit to node ids and values
    const auto *highway = node.get_value_by_key("highway");

    if (!highway || !Equal("motorway_junction", highway))
      return;

    const auto *exitTo = node.get_value_by_key("exit_to");

    if (exitTo)
      destinations.insert({node.positive_id(), std::string{exitTo}});
  }

  void way(const osmium::Way &way) {
    const auto &nodes = way.nodes();

    const auto *highway = way.get_value_by_key("highway");

    // http://wiki.openstreetmap.org/wiki/Key:highway#Link_roads
    const auto isLink = highway && (Equal("motorway_link", highway)      //
                                    || Equal("trunk_link", highway)      //
                                    || Equal("primary_link", highway)    //
                                    || Equal("secondary_link", highway)  //
                                    || Equal("tertiary_link", highway)); //

    const auto *oneway = way.get_value_by_key("oneway");
    const auto isOneway = oneway && (Equal("yes", oneway) || //
                                     Equal("1", oneway) ||   //
                                     Equal("true", oneway)); //

    const auto isReversed = isOneway && Equal("-1", oneway);
    const auto startNode = isReversed ? nodes.back().positive_ref() : nodes.front().positive_ref();

    const auto hasDestination = way.get_value_by_key("destination") != nullptr;

    Committer defer{outbuf};

    // Copy over way attributes untouched
    osmium::builder::WayBuilder builder{outbuf};
    Copy(builder, way);

    if (isLink && isOneway && !hasDestination) {
      const auto it = destinations.find(startNode);

      if (it != end(destinations)) {
        Extend(builder, way.tags(), "destination", it->second.c_str());
        added += 1;
      } else {
        builder.add_item(way.tags());
      }
    } else {
      builder.add_item(way.tags());
    }

    // Copy over way nodes untouched
    builder.add_item(way.nodes());
  }

  void relation(const osmium::Relation &relation) {
    Committer defer{outbuf};

    // Copy over relation data untouched
    outbuf.add_item(relation);
  }

  // We write and committ into a buffer; let the user grab the full buffer.
  // Switch in an empty one for the next handler application.
  osmium::memory::Buffer buffer() {
    osmium::memory::Buffer tmp{kBufferCapacityHint};

    using std::swap;
    swap(tmp, outbuf);

    return tmp;
  }

  // Number of destination way tags added.
  std::size_t added = 0;

private:
  osmium::memory::Buffer outbuf{kBufferCapacityHint};

  using NodeId = osmium::unsigned_object_id_type;
  using Value = std::string;

  std::unordered_map<NodeId, Value> destinations;
};

int main(int argc, char **argv) try {
  if (argc != 3) {
    std::fprintf(stderr, "Usage: %s in.osm.pbf out.osm.pbf\n", argv[0]);
    return EXIT_FAILURE;
  }

  osmium::io::File infile{argv[1]};
  osmium::io::File outfile{argv[2]};

  osmium::io::Reader reader{infile};
  osmium::io::Writer writer{outfile};

  ExitToRewriter xform;

  while (const auto inbuf = reader.read()) {
    osmium::apply(inbuf, xform);

    writer(xform.buffer());
  }

  writer.close();
  reader.close();

  std::fprintf(stdout, "Ok: added %zu destination tags\n", xform.added);

} catch (const std::exception &e) {
  std::fprintf(stderr, "Error: %s\n", e.what());
  return EXIT_FAILURE;
}
