#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset

readonly LIBOSMIUM_URL="https://github.com/osmcode/libosmium/archive/v2.12.2.tar.gz"
readonly LIBOSMIUM_DIR="libosmium"

mkdir -p third_party/${LIBOSMIUM_DIR}
pushd third_party
wget --quiet -O - ${LIBOSMIUM_URL} | tar --strip-components=1 -xz -C ${LIBOSMIUM_DIR}
popd
