project(NaivecoinNode VERSION 0.1 LANGUAGES CXX)

# Executable

add_executable(
    NaivecoinNode
    src/main.cpp
    src/node.cpp
    src/miner.cpp
    src/options.cpp
)

target_include_directories(
    NaivecoinNode
    PUBLIC
)

set_target_properties(
    NaivecoinNode
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    NaivecoinNode
    LINK_PUBLIC
    Threads::Threads
    boost-system
    boost-programoptions
    fmt::fmt
    spdlog::spdlog
    NaivecoinCore
    NaivecoinHttp
    NaivecoinTransaction
)


# Installation

install(
    TARGETS NaivecoinNode
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
