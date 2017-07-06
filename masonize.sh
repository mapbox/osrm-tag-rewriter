#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset


git clone --depth 1 --branch v0.13.0 https://github.com/mapbox/mason.git .mason
