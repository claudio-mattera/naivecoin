project(NaivecoinMiner VERSION 0.1 LANGUAGES CXX)

# Executable

add_executable(
    NaivecoinMiner
    src/main.cpp
)

target_include_directories(
    NaivecoinMiner
    PUBLIC
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIRS}
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
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    NaivecoinCore
)


# Installation

install(
    TARGETS NaivecoinMiner
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
