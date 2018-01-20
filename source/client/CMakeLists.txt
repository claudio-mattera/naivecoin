project(NaivecoinClient VERSION 0.1 LANGUAGES CXX)

# Executable

add_executable(
    NaivecoinClient
    src/main.cpp
    src/options.cpp
)

target_include_directories(
    NaivecoinClient
    PUBLIC
)

set_target_properties(
    NaivecoinClient
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    NaivecoinClient
    LINK_PUBLIC
    Threads::Threads
    boost-system
    boost-programoptions
    fmt::fmt
    spdlog::spdlog
    NaivecoinCore
    NaivecoinTransaction
    NaivecoinSerialization
)


# Installation

install(
    TARGETS NaivecoinClient
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
