
#pragma once

#ifndef NAIVECOIN_SERIALIZATION_BLOCK_H
#define NAIVECOIN_SERIALIZATION_BLOCK_H

#include <naivecoin/core/block.h>

namespace naivecoin::serialization {

naivecoin::core::Block deserialize_block(std::string const & text);

template<class Container>
void deserialize_blockchain(std::insert_iterator<Container> dest, std::string const & text);

template<template<class> class Iterator>
std::string serialize_blockchain(Iterator<naivecoin::core::Block> const begin, Iterator<naivecoin::core::Block> const end);

} // namespace naivecoin::serialization

#endif // NAIVECOIN_SERIALIZATION_BLOCK_H
