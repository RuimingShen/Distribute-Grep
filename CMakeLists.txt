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
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

## Own libraries
add_library(session 
    STATIC ${PROJECT_SOURCE_DIR}/session/session.cc
)

target_include_directories(session 
    PUBLIC ${PROJECT_SOURCE_DIR}/session
)

## Executables
# grep_server
add_executable(grep_server 
  ${CMAKE_SOURCE_DIR}/src/server/grep_server.cc
)

# query_client
add_executable(query_client 
  ${CMAKE_SOURCE_DIR}/src/client/query_client.cc
)

## Add include directories 
target_include_directories(grep_server
    PRIVATE ${PROJECT_INCLUDE_DIR}/session # need this since it does not include subdirectories
)

target_include_directories(query_client
    PRIVATE ${PROJECT_INCLUDE_DIR}/session
)

## Add link libraries: boost + pthread + our own library
target_link_libraries(grep_server 
    PRIVATE ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE session
)

target_link_libraries(query_client 
    PRIVATE ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    PRIVATE session
)

## Compile options
add_compile_options(-Wall -Wextra -O3)
