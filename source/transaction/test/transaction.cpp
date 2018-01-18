#include <boost/test/unit_test.hpp>

#include <list>

#include <naivecoin/transaction/transaction.h>

namespace {

std::pair<std::string, std::string> const first_pair = std::make_pair(
        "-----BEGIN PUBLIC KEY-----\n"
        "MIGbMBAGByqGSM49AgEGBSuBBAAjA4GGAAQBePh09METFzZarcK8L6ii9DQg0TOo\n"
        "ksUy4spAOzitEND/Paj4/0r9GINVw8E4R6M0UeKUMgfg56Iz++z/StGXR4kBIv9E\n"
        "Wykc0YkKzOfrLwqEZGBTZWWyc0zCmVLvpfjolpmRjGfSKR9RMCA2h6+G5a70tP/Z\n"
        "S2yRKNqxcOLbIHJd/Zw=\n"
        "-----END PUBLIC KEY-----\n"
    ,
        "-----BEGIN PRIVATE KEY-----\n"
        "MIHuAgEAMBAGByqGSM49AgEGBSuBBAAjBIHWMIHTAgEBBEIArUsioLrVthLqbwcG\n"
        "0zZ/9DrVWZrlRP9pLIawvRicsKWKVHc/r3HcqioAktXNVMZhCIz0r6Pn/F2pRKka\n"
        "TLWZLI2hgYkDgYYABAF4+HT0wRMXNlqtwrwvqKL0NCDRM6iSxTLiykA7OK0Q0P89\n"
        "qPj/Sv0Yg1XDwThHozRR4pQyB+DnojP77P9K0ZdHiQEi/0RbKRzRiQrM5+svCoRk\n"
        "YFNlZbJzTMKZUu+l+OiWmZGMZ9IpH1EwIDaHr4blrvS0/9lLbJEo2rFw4tsgcl39\n"
        "nA==\n"
        "-----END PRIVATE KEY-----\n"
);

std::pair<std::string, std::string> const second_pair = std::make_pair(
        "-----BEGIN PUBLIC KEY-----\n"
        "MIGbMBAGByqGSM49AgEGBSuBBAAjA4GGAAQBPLEXlBtalIBXtGb8JcMJf5suFJnS\n"
        "MN5BuLpJW+UJvlX/SP6S7r0xgcIMPFFI7SYWbHZSabIaVyUoIP0z59/cnyABEhdv\n"
        "feKQGeND5KoR7ddAFAb+od8HI+D+1z8Z3naa39VGoWv+HRPyv22MEBGNvnxrGL7c\n"
        "fJ2jshcQZxBGsDpNOi4=\n"
        "-----END PUBLIC KEY-----\n"
    ,
        "-----BEGIN PRIVATE KEY-----\n"
        "MIHuAgEAMBAGByqGSM49AgEGBSuBBAAjBIHWMIHTAgEBBEIBOpt59AqmqrKQRuqU\n"
        "T6K4IY1S4S+XGQ0P1LlcSUSeCpu/q/HdLP8wtMX1v25k6aL5VSojjBm9D+cIIjt1\n"
        "exhWgNChgYkDgYYABAE8sReUG1qUgFe0Zvwlwwl/my4UmdIw3kG4uklb5Qm+Vf9I\n"
        "/pLuvTGBwgw8UUjtJhZsdlJpshpXJSgg/TPn39yfIAESF2994pAZ40PkqhHt10AU\n"
        "Bv6h3wcj4P7XPxnedprf1Uaha/4dE/K/bYwQEY2+fGsYvtx8naOyFxBnEEawOk06\n"
        "Lg==\n"
        "-----END PRIVATE KEY-----\n"
);

std::pair<std::string, std::string> const third_pair = std::make_pair(
        "-----BEGIN PUBLIC KEY-----\n"
        "MIGbMBAGByqGSM49AgEGBSuBBAAjA4GGAAQB7Zpoq+K6cqz63EkBiYpjUbmtKsK1\n"
        "ZM9C2I+7wZD2HqiyQ2p3Y28XC4jejOVnkoz6TFI78dR9p4GGtXMXEXkaRSgA1GL0\n"
        "wSgP81RxyrM6Xu2G8Szb8DamB+n+GLrcBAQqRkNFce0gxKVJ9UbcCuhXawTcp34N\n"
        "ETAFNqn0QzN+3hISVRk=\n"
        "-----END PUBLIC KEY-----\n"
    ,
        "-----BEGIN PRIVATE KEY-----\n"
        "MIHuAgEAMBAGByqGSM49AgEGBSuBBAAjBIHWMIHTAgEBBEIB2+BJr0fOE+tL7wAV\n"
        "rbjh21g9IA/4zo1RH9mHfYCVBo7gKNb8LXpJejAEArQ3qzv1pUoNUfjngNdvmTWG\n"
        "fQX+FcehgYkDgYYABAHtmmir4rpyrPrcSQGJimNRua0qwrVkz0LYj7vBkPYeqLJD\n"
        "andjbxcLiN6M5WeSjPpMUjvx1H2ngYa1cxcReRpFKADUYvTBKA/zVHHKszpe7Ybx\n"
        "LNvwNqYH6f4YutwEBCpGQ0Vx7SDEpUn1RtwK6FdrBNynfg0RMAU2qfRDM37eEhJV\n"
        "GQ==\n"
        "-----END PRIVATE KEY-----\n"
);

std::pair<std::string, std::string> const fourth_pair = std::make_pair(
        "-----BEGIN PUBLIC KEY-----\n"
        "MIGbMBAGByqGSM49AgEGBSuBBAAjA4GGAAQAGx93mxusm6p5uflgV1ULYeuXb83A\n"
        "o7LuVnzUhnk7VZOB6Dep+NwF66EDi4Wr4eNVRQgWve2DOLTiZnMtmC/2XRoBRNSW\n"
        "v/u8cg9mvoG/RTn0wl/YQfdY8zfFfbvA7hI4EJ3Jl8vH6hTzgqVQTQGFEKNpa4hW\n"
        "LJKGyBWd2ItZpwr7/tc=\n"
        "-----END PUBLIC KEY-----\n"
    ,
        "-----BEGIN PRIVATE KEY-----\n"
        "MIHuAgEAMBAGByqGSM49AgEGBSuBBAAjBIHWMIHTAgEBBEIAtfczy94JhH4Acn0b\n"
        "sTMw2tA/8crMMrWeOKAUADKvHRgXE0JKC+ZHip+j9YgUn1mhM0lir2A1PwIz0e85\n"
        "Oudm43yhgYkDgYYABAAbH3ebG6ybqnm5+WBXVQth65dvzcCjsu5WfNSGeTtVk4Ho\n"
        "N6n43AXroQOLhavh41VFCBa97YM4tOJmcy2YL/ZdGgFE1Ja/+7xyD2a+gb9FOfTC\n"
        "X9hB91jzN8V9u8DuEjgQncmXy8fqFPOCpVBNAYUQo2lriFYskobIFZ3Yi1mnCvv+\n"
        "1w==\n"
        "-----END PRIVATE KEY-----\n"
);

std::pair<std::string, std::string> const fifth_pair = std::make_pair(
        "-----BEGIN PUBLIC KEY-----\n"
        "MIGbMBAGByqGSM49AgEGBSuBBAAjA4GGAAQB/dqZE70tj4jbiv32eihZ4eZ6Vgi2\n"
        "f+YlehjOFKUyHQFZQYjn/9bqc+iFw8b5LTd2BuIsIwVyErOXRrohMtHGqNoA82AX\n"
        "r/mA5EArP/sEGWS+HEzCMTQy4zzvmZMpmekizUI7zgKpd2GVaV6ZW/DY8WYMDm9a\n"
        "qQcReG+n3SzXtrPx/O0=\n"
        "-----END PUBLIC KEY-----\n"
    ,
        "-----BEGIN PRIVATE KEY-----\n"
        "MIHuAgEAMBAGByqGSM49AgEGBSuBBAAjBIHWMIHTAgEBBEIBUDKr4u7LCrdtL/on\n"
        "x3/ABLkU2NGJMNws/9GID8pD+SzYOv5ElEC5Zxfik9n987qP886SMShmLtNeuXMP\n"
        "s9BqrMyhgYkDgYYABAH92pkTvS2PiNuK/fZ6KFnh5npWCLZ/5iV6GM4UpTIdAVlB\n"
        "iOf/1upz6IXDxvktN3YG4iwjBXISs5dGuiEy0cao2gDzYBev+YDkQCs/+wQZZL4c\n"
        "TMIxNDLjPO+ZkymZ6SLNQjvOAql3YZVpXplb8NjxZgwOb1qpBxF4b6fdLNe2s/H8\n"
        "7Q==\n"
        "-----END PRIVATE KEY-----\n"
);

std::vector<std::string> const addresses{
    first_pair.first,
    second_pair.first,
    third_pair.first,
    fourth_pair.first,
    fifth_pair.first
};

std::vector<std::string> const private_keys{
    first_pair.second,
    second_pair.second,
    third_pair.second,
    fourth_pair.second,
    fifth_pair.second
};

std::pair<naivecoin::transaction::Transaction, std::list<naivecoin::transaction::UnspentOutput>> make_test_transaction()
{
    std::list<naivecoin::transaction::UnspentOutput> const unspent_outputs{
        naivecoin::transaction::UnspentOutput(
            "first transaction id",
            7,
            addresses[0],
            2874
        ),
        naivecoin::transaction::UnspentOutput(
            "second transaction id",
            34,
            addresses[1],
            13
        ),
        naivecoin::transaction::UnspentOutput(
            "third transaction id",
            239,
            addresses[2],
            983
        )
    };

    std::list<naivecoin::transaction::Input> inputs{
        naivecoin::transaction::Input(
            "first transaction id",
            7
        ),
        naivecoin::transaction::Input(
            "third transaction id",
            239
        )
    };

    std::list<naivecoin::transaction::Output> const outputs{
        naivecoin::transaction::Output(
            addresses[3],
            984
        ),
        naivecoin::transaction::Output(
            addresses[4],
            2873
        )
    };

    naivecoin::transaction::Transaction const transaction = naivecoin::transaction::create_transaction(
        inputs,
        outputs,
        {private_keys[0], private_keys[2]},
        unspent_outputs
    );

    return std::make_pair(transaction, unspent_outputs);
}

} // unnamed namespace

BOOST_AUTO_TEST_SUITE(TransactionTransaction)

BOOST_AUTO_TEST_CASE(compute_transaction_id)
{
    std::list<naivecoin::transaction::Input> const inputs{
        naivecoin::transaction::Input("some id", 23, ""),
        naivecoin::transaction::Input("some other id", 54, ""),
        naivecoin::transaction::Input("a third id", 458, "")
    };
    std::list<naivecoin::transaction::Output> const outputs{
        naivecoin::transaction::Output("12345", 7564),
        naivecoin::transaction::Output("98765", 546),
        naivecoin::transaction::Output("0001", 5436)
    };

    naivecoin::transaction::Transaction transaction("", inputs, outputs);

    std::string const actual = naivecoin::transaction::compute_transaction_id(transaction);

    std::string const expected = "f515a03fd85da81a23ea16b227678fdd7ec99de8";

    BOOST_CHECK_EQUAL(actual, expected);
}

BOOST_AUTO_TEST_CASE(is_transaction_valid)
{
    auto pair = make_test_transaction();
    naivecoin::transaction::Transaction transaction = pair.first;
    std::list<naivecoin::transaction::UnspentOutput> unspent_outputs = pair.second;

    bool const is_valid = naivecoin::transaction::is_transaction_valid(transaction, unspent_outputs);

    BOOST_CHECK(is_valid);
}

BOOST_AUTO_TEST_CASE(is_transaction_valid_coinbase)
{
    uint64_t const index = 33;
    naivecoin::transaction::Transaction transaction = naivecoin::transaction::create_coinbase_transaction(index, addresses[3]);
    bool const is_valid = naivecoin::transaction::is_transaction_valid(transaction, {});

    BOOST_CHECK(! is_valid);
}

BOOST_AUTO_TEST_CASE(is_coinbase_transaction_valid)
{
    uint64_t const index = 33;
    naivecoin::transaction::Transaction const transaction = naivecoin::transaction::create_coinbase_transaction(index, addresses[3]);
    bool const is_valid = naivecoin::transaction::is_coinbase_transaction_valid(index, transaction);

    BOOST_CHECK(is_valid);
}

BOOST_AUTO_TEST_CASE(is_transaction_list_valid)
{
    uint64_t const index = 33;
    auto pair = make_test_transaction();
    naivecoin::transaction::Transaction transaction = pair.first;
    std::list<naivecoin::transaction::UnspentOutput> unspent_outputs = pair.second;

    naivecoin::transaction::Transaction coinbase_transaction = naivecoin::transaction::create_coinbase_transaction(index, addresses[3]);

    std::list<naivecoin::transaction::Transaction> transactions{coinbase_transaction, transaction};

    bool const is_valid = naivecoin::transaction::is_transaction_list_valid(index, transactions, unspent_outputs);

    BOOST_CHECK(is_valid);
}

BOOST_AUTO_TEST_CASE(update_unspent_outputs)
{
    uint64_t const index = 7;
    std::list<naivecoin::transaction::UnspentOutput> const old_unspent_outputs{
        naivecoin::transaction::UnspentOutput(
            "first transaction",
            4,
            "first_address",
            10
        ),
        naivecoin::transaction::UnspentOutput(
            "second transaction",
            5,
            "second_address",
            12
        ),
        naivecoin::transaction::UnspentOutput(
            "third transaction",
            6,
            "first_address",
            4
        )
    };

    std::list<naivecoin::transaction::Transaction> const new_transactions{
        naivecoin::transaction::create_coinbase_transaction(
            index,
            "first address"
        ),
        naivecoin::transaction::Transaction(
            "fourth transaction",
            {
                naivecoin::transaction::Input{
                    "first transaction",
                    4
                },
                naivecoin::transaction::Input{
                    "third transaction",
                    6
                }
            },
            {
                naivecoin::transaction::Output{
                    "second address",
                    5
                },
                naivecoin::transaction::Output{
                    "first address",
                    9
                }
            }
        )
    };

    std::list<naivecoin::transaction::UnspentOutput> const expected_new_unspent_outputs{
        naivecoin::transaction::UnspentOutput(
            "second transaction",
            5,
            "second_address",
            12
        ),
        naivecoin::transaction::UnspentOutput{
            "e271e7e7916813e486628efc56ca7ce9176f380e",
            index,
            "first address",
            naivecoin::transaction::COINBASE_AMOUNT
        },
        naivecoin::transaction::UnspentOutput{
            "fourth transaction",
            index,
            "second address",
            5
        },
        naivecoin::transaction::UnspentOutput{
            "fourth transaction",
            index,
            "first address",
            9
        }
    };

    std::list<naivecoin::transaction::UnspentOutput> const new_unspent_outputs = naivecoin::transaction::update_unspent_outputs(
        index,
        new_transactions,
        old_unspent_outputs
    );

    BOOST_CHECK_EQUAL(new_unspent_outputs.size(), expected_new_unspent_outputs.size());

    auto actual_it = std::begin(new_unspent_outputs);
    auto expected_it = std::begin(expected_new_unspent_outputs);

    while (actual_it != std::end(new_unspent_outputs)) {
        BOOST_CHECK_EQUAL(actual_it->transaction_id, expected_it->transaction_id);
        BOOST_CHECK_EQUAL(actual_it->transaction_index, expected_it->transaction_index);
        BOOST_CHECK_EQUAL(actual_it->address, expected_it->address);
        BOOST_CHECK_EQUAL(actual_it->amount, expected_it->amount);

        ++actual_it;
        ++expected_it;
    }
}

BOOST_AUTO_TEST_SUITE_END()
