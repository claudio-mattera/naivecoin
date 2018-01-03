project(NaivecoinTransaction VERSION 0.1 LANGUAGES CXX)

# Library

add_library(
    NaivecoinTransaction
    SHARED
    src/output.cpp
    src/input.cpp
    src/transaction.cpp
)

target_include_directories(
    NaivecoinTransaction
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    src
)

set_target_properties(
    NaivecoinTransaction
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    NaivecoinTransaction
    LINK_PUBLIC
    boost
    openssl
    jsoncpp_lib
)


# Installation

install(
    TARGETS NaivecoinTransaction EXPORT NaivecoinTransactionConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # This is for Windows
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT NaivecoinTransactionConfig DESTINATION share/naivecoin/cmake)

# This makes the project importable from the build directory
export(TARGETS NaivecoinTransaction FILE NaivecoinTransactionConfig.cmake)


# Testing

add_executable(
    test-naivecoin-transaction
    test/main.cpp
    test/output.cpp
    test/input.cpp
    test/transaction.cpp
)

target_link_libraries(
    test-naivecoin-transaction
    boost-test
    NaivecoinTransaction
    openssl
)

add_test(test-naivecoin-transaction test-naivecoin-transaction)
