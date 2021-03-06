project(NaivecoinCrypto VERSION 0.1 LANGUAGES CXX)

# Library

add_library(
    NaivecoinCrypto
    SHARED
    src/crypto.cpp
)

target_include_directories(
    NaivecoinCrypto
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    src
)

set_target_properties(
    NaivecoinCrypto
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    NaivecoinCrypto
    LINK_PUBLIC
    openssl
)


# Installation

install(
    TARGETS NaivecoinCrypto EXPORT NaivecoinCryptoConfig
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # This is for Windows
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT NaivecoinCryptoConfig DESTINATION share/naivecoin/cmake)

# This makes the project importable from the build directory
export(TARGETS NaivecoinCrypto FILE NaivecoinCryptoConfig.cmake)


# Testing

if(BUILD_TESTING)
    add_executable(
        NaivecoinCryptoTest
        test/main.cpp
        test/crypto.cpp
    )

    set_target_properties(
        NaivecoinCryptoTest
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )

    target_link_libraries(
        NaivecoinCryptoTest
        boost-test
        NaivecoinCrypto
    )

    add_test(NaivecoinCryptoTest NaivecoinCryptoTest)
endif(BUILD_TESTING)
