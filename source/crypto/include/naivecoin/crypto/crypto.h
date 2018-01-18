
#pragma once

#ifndef NAIVECOIN_CRYPTO_CRYPTO_H
#define NAIVECOIN_CRYPTO_CRYPTO_H

#include <string>
#include <utility>

namespace naivecoin::crypto {

std::string compute_hash(std::string const & data);

std::pair<std::string, std::string> generate_key_pair();

std::string sign(std::string const & data, std::string const & private_key);
bool verify(std::string const & data, std::string const & signature, std::string const & public_key);

} // namespace naivecoin::crypto

#endif // NAIVECOIN_CRYPTO_CRYPTO_H
