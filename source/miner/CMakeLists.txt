project(NaivecoinMiner VERSION 0.1 LANGUAGES CXX)

# Executable

add_executable(
    NaivecoinMiner
    src/main.cpp
    src/controlserver.cpp
    src/controlconnection.cpp
    src/dataserver.cpp
    src/dataconnection.cpp
    src/miner.cpp
    src/options.cpp
)

target_include_directories(
    NaivecoinMiner
    PUBLIC
)

set_target_properties(
    NaivecoinMiner
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    NaivecoinMiner
    LINK_PUBLIC
    Threads::Threads
    boost-system
    boost-programoptions
    NaivecoinCore
    NaivecoinHttp
)


# Installation

install(
    TARGETS NaivecoinMiner
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
