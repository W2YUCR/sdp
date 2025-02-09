cmake_minimum_required(VERSION 3.28) # For modules support
project(state-driven-parsing)

set(CMAKE_CXX_STANDARD 23)

find_package(mio CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS flyweight)
find_package(Catch2 CONFIG REQUIRED)
find_package(libassert CONFIG REQUIRED)

include(CTest)
include(Catch)

add_subdirectory(grammar)

add_executable(ll-table-gen)
target_sources(ll-table-gen PUBLIC FILE_SET CXX_MODULES FILES ll-table-gen.cppm)
target_link_libraries(ll-table-gen PRIVATE Grammar mio::mio)