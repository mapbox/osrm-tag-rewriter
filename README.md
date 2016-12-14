## `exit_to=` to `destination=`

Rewrites OpenStreetMap [`exit_to=`](http://wiki.openstreetmap.org/wiki/Key:exit_to) Node tags on [`highway=motorway_junction`](https://wiki.openstreetmap.org/wiki/Tag:highway%3Dmotorway_junction) to [`destination=`](http://wiki.openstreetmap.org/wiki/Key:destination) tags on the adjacent way when possible to do so without ambiguity.

### Why

- Query for `exit_to` node tags, Bay Area: http://overpass-turbo.eu/s/kEM
- Query for `destination` way tags, Bay Area: http://overpass-turbo.eu/s/kEN

## Building

A recent libosmium is getting downloaded to `third_party` by invoking the `./deps.sh` script.

    ./deps.sh
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build .

## Running

    ./exit2destination in.osm.pbf out.osm.pbf

## License

Copyright © 2016 Mapbox

Distributed under the MIT License (MIT).
