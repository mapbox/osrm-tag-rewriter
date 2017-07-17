#include <cstdio>
#include <cstdlib>
#include <utility>

#include <osmium/io/pbf_input.hpp>
#include <osmium/io/pbf_output.hpp>
#include <osmium/io/reader.hpp>
#include <osmium/io/writer.hpp>
#include <osmium/io/xml_input.hpp>
#include <osmium/io/xml_output.hpp>
#include <osmium/visitor.hpp>

#include "destinations.h"
#include "exits.h"

int main(int argc, char **argv) try {
  if (argc != 3) {
    std::fprintf(stderr, "Usage: %s in.osm.pbf out.osm.pbf\n", argv[0]);
    return EXIT_FAILURE;
  }

  osmium::io::File infile{argv[1]};
  osmium::io::File outfile{argv[2]};

  osmium::io::Reader reader{infile};
  osmium::io::Writer writer{outfile};

  DestinationsRewriter destinationsRewriter;
  ExitsRewriter exitsRewriter;

  while (const auto inbuf = reader.read()) {
    // Todo: refactor as n-stages pipeline with passes enabled/disabled from cmdline

    osmium::apply(inbuf, destinationsRewriter);
    auto tmpbuf = destinationsRewriter.buffer();

    osmium::apply(tmpbuf, exitsRewriter);
    auto outbuf = exitsRewriter.buffer();

    writer(std::move(outbuf));
  }

  writer.close();
  reader.close();

  std::fprintf(stdout, "Ok: added %zu destination tags\n", destinationsRewriter.added);
  std::fprintf(stdout, "Ok: added %zu junction:ref tags\n", exitsRewriter.added);

} catch (const std::exception &e) {
  std::fprintf(stderr, "Error: %s\n", e.what());
  return EXIT_FAILURE;
}
