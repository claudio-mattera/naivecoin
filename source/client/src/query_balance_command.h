
#pragma once

#ifndef NAIVECOIN_CLIENT_QUERYBALANCECOMMAND_H
#define NAIVECOIN_CLIENT_QUERYBALANCECOMMAND_H

#include <utility>
#include <functional>

#include <boost/program_options.hpp>

std::pair<
    boost::program_options::options_description,
    std::function<int(boost::program_options::variables_map const &)>
> get_query_balance_pair();

#endif // NAIVECOIN_CLIENT_QUERYBALANCECOMMAND_H
