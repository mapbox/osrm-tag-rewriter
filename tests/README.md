Tests are written in `rewrite.feature` and then converted to `.osm` files using the `osrm-backend` Cucumber test suite.
The `check.sh` script makes sure positive tests add tags and negative tests do not add tags.
