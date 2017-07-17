#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset

readonly rewrite=$(realpath ${1:-../build/osrm-tag-rewriter})

${rewrite} positive/8_successful_rewrite.osm positive/8_successful_rewrite_rewritten.osm
${rewrite} positive/157_reversed_way.osm positive/157_reversed_way_rewritten.osm

${rewrite} negative/29_left_and_right_exits_not_supported_only_a_few_hundred_left_right.osm negative/29_left_and_right_exits_not_supported_only_a_few_hundred_left_right_rewritten.osm
${rewrite} negative/52_not_a_motorway_junction_node.osm negative/52_not_a_motorway_junction_node_rewritten.osm
${rewrite} negative/73_not_a_link_road.osm negative/73_not_a_link_road_rewritten.osm
${rewrite} negative/94_not_a_oneway_link.osm negative/94_not_a_oneway_link_rewritten.osm
${rewrite} negative/115_tag_already_present.osm negative/115_tag_already_present_rewritten.osm
${rewrite} negative/136_ref_tag_already_present.osm negative/136_ref_tag_already_present_rewritten.osm
