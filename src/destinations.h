#pragma once

#include <cstdlib>

#include <string>
#include <unordered_map>
#include <utility>

#include <osmium/builder/osm_object_builder.hpp>
#include <osmium/handler.hpp>
#include <osmium/osm/types.hpp>
#include <osmium/tags/taglist.hpp>
#include <osmium/tags/tags_filter.hpp>
#include <osmium/util/string_matcher.hpp>

#include "util.h"

// Collects node ids and exit_to=* tags on highway=motorway_junction.
// http://wiki.openstreetmap.org/wiki/Key:exit_to
// http://taginfo.openstreetmap.org/keys/exit_to
struct DestinationsRewriter : osmium::handler::Handler {
  static const constexpr auto kExitToTagsInPlanet = 35000u;
  static const constexpr auto kBufferCapacityHint = 1u << 12u;

  DestinationsRewriter() { destinations.reserve(kExitToTagsInPlanet); }

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
    const auto isOneway = oneway && (Equal("yes", oneway) ||  //
                                     Equal("1", oneway) ||    //
                                     Equal("true", oneway) || //
                                     Equal("-1", oneway));    //

    const auto isReversed = isOneway && Equal("-1", oneway);
    const auto startNode = isReversed ? nodes.back().positive_ref() : nodes.front().positive_ref();

    // Do not re-write if there are "destination*" tags already present
    osmium::TagsFilter destinationFilter;
    osmium::TagMatcher destinationMatcher{osmium::StringMatcher::prefix{"destination"}};
    destinationFilter.add_rule(true, destinationMatcher);

    const auto hasDestination = osmium::tags::match_any_of(way.tags(), destinationFilter);

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
