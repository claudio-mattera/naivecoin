
#pragma once

#ifndef NAIVECOIN_NODE_OPTIONS_H
#define NAIVECOIN_NODE_OPTIONS_H

#include <boost/program_options.hpp>

namespace naivecoin::node {

boost::program_options::variables_map process_program_options(int const argc, char const * const argv[]);

} // namespace naivecoin::node

#endif // NAIVECOIN_NODE_OPTIONS_H
