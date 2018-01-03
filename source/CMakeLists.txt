cmake_minimum_required(VERSION 3.8)
project(naivecoin VERSION 0.1 LANGUAGES CXX)

include(GNUInstallDirs)


# Dependencies

# Boost
find_package(Boost COMPONENTS system log thread unit_test_framework REQUIRED )

add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

add_library(boost-test SHARED IMPORTED)
set_property(TARGET boost-test PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(TARGET boost-test PROPERTY IMPORTED_LOCATION ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_library(boost-system SHARED IMPORTED)
set_property(TARGET boost-system PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(TARGET boost-system PROPERTY IMPORTED_LOCATION ${Boost_SYSTEM_LIBRARY})

# OpenSSL
find_package(OpenSSL)
add_library(openssl SHARED IMPORTED)
set_property(TARGET openssl PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
set_property(TARGET openssl PROPERTY IMPORTED_LOCATION ${OPENSSL_SSL_LIBRARY})

# Threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# JSON CPP
find_package(jsoncpp)


# Subprojects

enable_testing()
add_subdirectory(core)
add_subdirectory(transaction)
add_subdirectory(miner)
