#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset

readonly bin=$(realpath ${1:-../build/exit2destination})

for osm in positive/*.osm
do
	code=$(${bin} ${osm} ${osm%.*}_rewritten.osm | egrep --only-matching '[[:digit:]]')
	if [ ${code} -lt 1 ]; then echo "Error: ${osm}"; exit 1; fi
done

for osm in negative/*.osm
do
	code=$(${bin} ${osm} ${osm%.*}_rewritten.osm | egrep --only-matching '[[:digit:]]')
	if [ ${code} -gt 0 ]; then echo "Error: ${osm}"; exit 1; fi
done
