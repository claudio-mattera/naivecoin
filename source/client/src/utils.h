
#pragma once

#ifndef NAIVECOIN_CLIENT_UTILS_H
#define NAIVECOIN_CLIENT_UTILS_H

#include <string>

void check_port(uint64_t const port);
void check_socket(std::string const & value);
std::string get_address(std::string const & private_key);
std::string read_file(std::string const & filename);

#endif // NAIVECOIN_CLIENT_UTILS_H
