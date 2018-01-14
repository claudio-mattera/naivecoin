
#pragma once

#ifndef NAIVECOIN_CLIENT_OPTIONS_H
#define NAIVECOIN_CLIENT_OPTIONS_H

#include <boost/program_options.hpp>

namespace naivecoin {

boost::program_options::variables_map process_program_options(int const argc, char const * const argv[]);

} // namespace naivecoin

#endif // NAIVECOIN_CLIENT_OPTIONS_H
