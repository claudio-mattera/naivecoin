project(NaivecoinSerialization VERSION 0.1 LANGUAGES CXX)

# Library

add_library(
    NaivecoinSerialization
    SHARED
    src/common.cpp
    src/time.cpp
    src/block.cpp
    src/message.cpp
    src/transaction.cpp
)

target_include_directories(
    NaivecoinSerialization
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    src
)

set_target_properties(
    NaivecoinSerialization
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    NaivecoinSerialization
    LINK_PUBLIC
    jsoncpp_lib
    NaivecoinCrypto
    NaivecoinCore
    NaivecoinTransaction
)


# Installation

install(
    TARGETS NaivecoinSerialization EXPORT NaivecoinSerializationConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # This is for Windows
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT NaivecoinSerializationConfig DESTINATION share/naivecoin/cmake)

# This makes the project importable from the build directory
export(TARGETS NaivecoinSerialization FILE NaivecoinSerializationConfig.cmake)


# Testing

add_executable(
    NaivecoinSerializationTest
    test/main.cpp
    test/block.cpp
    test/message.cpp
    test/transaction.cpp
)

set_target_properties(
    NaivecoinSerializationTest
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    NaivecoinSerializationTest
    boost-test
    NaivecoinSerialization
)

add_test(NaivecoinSerializationTest NaivecoinSerializationTest)
