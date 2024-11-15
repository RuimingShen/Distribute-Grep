cmake_minimum_required(VERSION 3.14)

project(distributed_log_system LANGUAGES CXX)

set(PROJECT_BINARY_DIR  build/bin)
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(PROJECT_SOURCE_DIR  ${CMAKE_SOURCE_DIR}/src)

## Binary paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Boost library
find_package(Boost REQUIRED COMPONENTS system thread filesystem)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

## GTest
find_package(GTest REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

## Own libraries
add_library(grep_server_lib STATIC 
    ${PROJECT_SOURCE_DIR}/server/grep_server_session.cc
    ${PROJECT_SOURCE_DIR}/server/grep_server.cc
)

add_library(query_client_lib STATIC 
    ${PROJECT_SOURCE_DIR}/client/query_client_session.cc
    ${PROJECT_SOURCE_DIR}/client/query_client.cc
)

## Executables
# grep_server
add_executable(grep_server 
  ${CMAKE_SOURCE_DIR}/src/server/grep_server_main.cc
)

# query_client
add_executable(query_client 
  ${CMAKE_SOURCE_DIR}/src/client/query_client_main.cc
)

## Add link libraries: boost + pthread + our own library
target_link_libraries(grep_server 
    PRIVATE ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE grep_server_lib
)

target_link_libraries(query_client 
    PRIVATE ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE query_client_lib
)

## Compile options
add_compile_options(-Wall -Wextra -O3)

## For Google test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

cmake_policy(SET CMP0135 NEW)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(mp1_test 
  ${CMAKE_SOURCE_DIR}/src/client/query_client.cc
  ${CMAKE_SOURCE_DIR}/test/mp1_test.cc
)

target_link_libraries(mp1_test 
    PRIVATE ${Boost_LIBRARIES}
    PRIVATE query_client_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(mp1_test)

