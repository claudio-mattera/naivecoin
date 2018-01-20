project(NaivecoinCore VERSION 0.1 LANGUAGES CXX)

# Library

add_library(
    NaivecoinCore
    SHARED
    src/block.cpp
    src/utils.cpp
)

target_include_directories(
    NaivecoinCore
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    src
)

set_target_properties(
    NaivecoinCore
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    NaivecoinCore
    LINK_PUBLIC
    NaivecoinCrypto
    jsoncpp_lib
)


# Installation

install(
    TARGETS NaivecoinCore EXPORT NaivecoinCoreConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # This is for Windows
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT NaivecoinCoreConfig DESTINATION share/naivecoin/cmake)

# This makes the project importable from the build directory
export(TARGETS NaivecoinCore FILE NaivecoinCoreConfig.cmake)


# Testing

add_executable(
    NaivecoinCoreTest
    test/main.cpp
    test/block.cpp
    test/utils.cpp
)

set_target_properties(
    NaivecoinCoreTest
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    NaivecoinCoreTest
    boost-test
    NaivecoinCore
)

add_test(NaivecoinCoreTest NaivecoinCoreTest)
