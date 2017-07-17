## osrm-tag-rewriter

Rewrites OpenStreetMap tags for example from motorway junction nodes to its ramps.
Mainly for easier handling in our [routing engine](https://github.com/Project-OSRM/osrm-backend#open-source-routing-machine).

[![Continuous Integration](https://travis-ci.org/mapbox/osrm-tag-rewriter.svg?branch=master)](https://travis-ci.org/mapbox/osrm-tag-rewriter)


## Building

A recent libosmium is getting downloaded to `third_party` by invoking the `./deps.sh` script.

    ./deps.sh
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build .

## Using Mason

You can build this project using packages bundled with mason:

    ./masonize.sh
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_MASON=On
    cmake --build .

## Running

    ./osrm-tag-rewriter in.osm.pbf out.osm.pbf

## Tests

See the `tests` directory.

    pushd tests
    ./check.sh
    popd


## Rewriters

### Destination Tags

Rewrites OpenStreetMap [`exit_to=`](http://wiki.openstreetmap.org/wiki/Key:exit_to) Node tags on [`highway=motorway_junction`](https://wiki.openstreetmap.org/wiki/Tag:highway%3Dmotorway_junction) to [`destination=`](http://wiki.openstreetmap.org/wiki/Key:destination) tags on the adjacent way when possible to do so without ambiguity.

- Query for `exit_to` node tags, Bay Area: http://overpass-turbo.eu/s/kEM
- Query for `destination` way tags, Bay Area: http://overpass-turbo.eu/s/kEN


### Exit Numbers

Rewrites OpenStreetMap [`ref=`](http://wiki.openstreetmap.org/wiki/Key:ref) Node tags on [`highway=motorway_junction`](https://wiki.openstreetmap.org/wiki/Tag:highway%3Dmotorway_junction) to [`junction:ref=`](http://wiki.openstreetmap.org/wiki/Proposed_features/junction_details) tags on the adjacent way when possible to do so without ambiguity.



## License

Copyright © 2017 Mapbox

Distributed under the MIT License (MIT).
