cmake_minimum_required(VERSION 3.15)
project(soft-renderer)

# Include the Conan generated toolchain file
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
message("CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR}")

# find_package(cJSON REQUIRED)

file(GLOB_RECURSE ALL_SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.c" "${CMAKE_SOURCE_DIR}/third_party/*.cpp" "${CMAKE_SOURCE_DIR}/third_party/*.c")
file(GLOB_RECURSE ALL_HEADER_FILES "${CMAKE_SOURCE_DIR}/src/*.hpp" "${CMAKE_SOURCE_DIR}/src/*.h" "${CMAKE_SOURCE_DIR}/third_party/*.hpp" "${CMAKE_SOURCE_DIR}/third_party/*.h")

# include_directories(${CMAKE_SOURCE_DIR}/src  ${CMAKE_SOURCE_DIR}/include)
aux_source_directory(. ALL_SRC_FILES)

# add_executable(json_test main.cpp)
# target_link_libraries(json_test cjson::cjson)

add_executable(main ${ALL_SRC_FILES} ${ALL_HEADER_FILES})