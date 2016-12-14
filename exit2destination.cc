#include <cstdlib>
#include <cstring>

#include <string>
#include <unordered_map>
#include <utility>

#include <osmium/builder/osm_object_builder.hpp>
#include <osmium/handler.hpp>
#include <osmium/io/input_iterator.hpp>
#include <osmium/io/output_iterator.hpp>
#include <osmium/io/pbf_input.hpp>
#include <osmium/io/pbf_output.hpp>
#include <osmium/io/reader.hpp>
#include <osmium/io/writer.hpp>
#include <osmium/osm/types.hpp>
#include <osmium/visitor.hpp>

// Ideas for improvement:
//  - in the way() function exit early when not highway
//  - inject output buffer size hint based on input buffer .committed() + expected destination tag length
//  - handle ref and other tags on the motorway junction node in the same way (check first if worth it)
//  - combine with planet pre-filtering: think filtering relations other than turn restrictions, buildings, trees

using NodeId = osmium::unsigned_object_id_type;
using WayId = osmium::unsigned_object_id_type;

template <typename Builder> //
inline void copyAttributes(Builder &builder, const osmium::OSMObject &object) {
  builder.set_id(object.id())
      .set_version(object.version())
      .set_changeset(object.changeset())
      .set_timestamp(object.timestamp())
      .set_uid(object.uid())
      .set_user(object.user());
}

inline void copyTags(osmium::builder::Builder &parent, const osmium::TagList &tags) { parent.add_item(tags); }

inline void copyTagsAddDestination(osmium::builder::Builder &parent, const osmium::TagList &tags,
                                   const std::string &destination) {
  osmium::builder::TagListBuilder builder{parent};

  for (const auto &tag : tags)
    builder.add_tag(tag);

  builder.add_tag("destination", destination.c_str());
}

// Collects node ids and exit_to=* tags on highway=motorway_junction.
// http://wiki.openstreetmap.org/wiki/Key:exit_to
// http://taginfo.openstreetmap.org/keys/exit_to
struct ExitToRewriter : osmium::handler::Handler {
  ExitToRewriter() {
    nodesToDestination.reserve(35000); // max in Planet
  }

  void node(const osmium::Node &node) {
    // Copy over node data untouched
    outbuf.add_item(node);
    outbuf.commit();

    // Record exit to node ids and values
    {
      const auto *highway = node.get_value_by_key("highway");

      if (!highway)
        return;

      const auto *exitTo = node.get_value_by_key("exit_to");

      if (!exitTo)
        return;

      const auto isJunction = std::strcmp("motorway_junction", highway) == 0;

      if (!isJunction)
        return;

      nodesToDestination.insert({node.positive_id(), std::string{exitTo}});
    }
  }

  void way(const osmium::Way &way) {
    const auto &nodes = way.nodes();

    const auto *highway = way.get_value_by_key("highway");
    const auto *oneway = way.get_value_by_key("oneway");
    const auto *destination = way.get_value_by_key("destination");

    const auto isOneway = oneway && (std::strcmp("yes", oneway) == 0 || //
                                     std::strcmp("1", oneway) == 0 ||   //
                                     std::strcmp("true", oneway) == 0); //

    const auto isReversed = isOneway && std::strcmp("-1", oneway) == 0;

    // http://wiki.openstreetmap.org/wiki/Key:highway#Link_roads
    const auto isLink = highway && (std::strcmp("motorway_link", highway) == 0      //
                                    || std::strcmp("trunk_link", highway) == 0      //
                                    || std::strcmp("primary_link", highway) == 0    //
                                    || std::strcmp("secondary_link", highway) == 0  //
                                    || std::strcmp("tertiary_link", highway) == 0); //

    const auto startNode = isReversed ? nodes.back().positive_ref() : nodes.front().positive_ref();

    {
      osmium::builder::WayBuilder builder{outbuf};
      copyAttributes(builder, way);

      if (!destination && isLink && isOneway) {
        auto it = nodesToDestination.find(startNode);

        if (it != end(nodesToDestination))
          copyTagsAddDestination(builder, way.tags(), it->second);
        else
          copyTags(builder, way.tags());
      } else {
        copyTags(builder, way.tags());
      }

      builder.add_item(way.nodes());
    }

    outbuf.commit();
  }

  void relation(const osmium::Relation &relation) {
    // Copy over relation data untouched
    outbuf.add_item(relation);
    outbuf.commit();
  }

  osmium::memory::Buffer getBuffer() {
    osmium::memory::Buffer tmp{4096};

    using std::swap;
    swap(tmp, outbuf);

    return tmp;
  }

  osmium::memory::Buffer outbuf{4096};
  std::unordered_map<NodeId, std::string> nodesToDestination;
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

  ExitToRewriter rewriter;

  while (const auto inbuf = reader.read()) {
    osmium::apply(inbuf, rewriter);

    writer(rewriter.getBuffer());
  }

  writer.close();
  reader.close();

} catch (const std::exception &e) {
  std::fprintf(stderr, "Error: %s\n", e.what());
  return EXIT_FAILURE;
}
