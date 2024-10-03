cmake_minimum_required(VERSION 3.20.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wno-ignored-attributes)

project(VectorizedCompression)

enable_testing()
include(GoogleTest)

include_directories(build/c++/third_party/pod5-file-format/c++)
add_subdirectory(c++)
