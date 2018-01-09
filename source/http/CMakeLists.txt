project(NaivecoinHttp VERSION 0.1 LANGUAGES CXX)

# Library

add_library(
    NaivecoinHttp
    SHARED
    src/http.cpp
)

target_include_directories(
    NaivecoinHttp
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    src
)

set_target_properties(
    NaivecoinHttp
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)


# Installation

install(
    TARGETS NaivecoinHttp EXPORT NaivecoinHttpConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # This is for Windows
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT NaivecoinHttpConfig DESTINATION share/naivecoin/cmake)

# This makes the project importable from the build directory
export(TARGETS NaivecoinHttp FILE NaivecoinHttpConfig.cmake)


# Testing

add_executable(
    NaivecoinHttpTest
    test/main.cpp
    test/http.cpp
)

target_link_libraries(
    NaivecoinHttpTest
    boost-test
    NaivecoinHttp
)

add_test(NaivecoinHttpTest NaivecoinHttpTest)
