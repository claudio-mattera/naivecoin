project(NaivecoinTime VERSION 0.1 LANGUAGES CXX)

# Library

add_library(
    NaivecoinTime
    SHARED
    src/time.cpp
)

target_include_directories(
    NaivecoinTime
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    src
)

set_target_properties(
    NaivecoinTime
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)


# Installation

install(
    TARGETS NaivecoinTime EXPORT NaivecoinTimeConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # This is for Windows
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT NaivecoinTimeConfig DESTINATION share/naivecoin/cmake)

# This makes the project importable from the build directory
export(TARGETS NaivecoinTime FILE NaivecoinTimeConfig.cmake)


# Testing

add_executable(
    NaivecoinTimeTest
    test/main.cpp
    test/time.cpp
)

set_target_properties(
    NaivecoinTimeTest
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    NaivecoinTimeTest
    boost-test
    NaivecoinTime
)

add_test(NaivecoinTimeTest NaivecoinTimeTest)
