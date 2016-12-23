#include <cstdio>
#include <cstdlib>
#include <utility>

#include <osmium/io/pbf_output.hpp>
#include <osmium/io/pbf_output.hpp>
#include <osmium/io/reader.hpp>
#include <osmium/io/writer.hpp>
#include <osmium/io/xml_input.hpp>
#include <osmium/io/xml_output.hpp>
#include <osmium/visitor.hpp>

#include "rewriter.h"

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

    auto outbuf = xform.buffer();

    writer(std::move(outbuf));
  }

  writer.close();
  reader.close();

  std::fprintf(stdout, "Ok: added %zu destination tags\n", xform.added);

} catch (const std::exception &e) {
  std::fprintf(stderr, "Error: %s\n", e.what());
  return EXIT_FAILURE;
}
