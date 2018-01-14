cmake_minimum_required(VERSION 3.8)
project(naivecoin VERSION 0.1 LANGUAGES CXX)

include(GNUInstallDirs)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wextra")
endif()

# Dependencies

# Boost
find_package(Boost COMPONENTS system log thread unit_test_framework program_options REQUIRED )

add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

add_library(boost-test SHARED IMPORTED)
set_property(TARGET boost-test PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(TARGET boost-test PROPERTY IMPORTED_LOCATION ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_library(boost-system SHARED IMPORTED)
set_property(TARGET boost-system PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(TARGET boost-system PROPERTY IMPORTED_LOCATION ${Boost_SYSTEM_LIBRARY})

add_library(boost-programoptions SHARED IMPORTED)
set_property(TARGET boost-programoptions PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(TARGET boost-programoptions PROPERTY IMPORTED_LOCATION ${Boost_PROGRAM_OPTIONS_LIBRARY})

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

# fmt
find_package(fmt CONFIG REQUIRED)

# Spdlog
find_package(spdlog CONFIG REQUIRED)


# Subprojects

enable_testing()
add_subdirectory(core)
add_subdirectory(crypto)
add_subdirectory(transaction)
add_subdirectory(http)
add_subdirectory(node)
add_subdirectory(client)
