include_directories(third_party/pod5-file-format/c++)

add_subdirectory(third_party)

set(POD5_SOURCE_DIR ${POD5_SOURCE_DIR} PARENT_SCOPE)


include_directories(SYSTEM ${POD5_SOURCE_DIR}/include)
include_directories(SYSTEM third_party/gsl-lite)
link_directories(${POD5_SOURCE_DIR}/lib)

message(CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}")

link_directories(${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)

set(POD5_LIBRARY_DIR ${POD5_SOURCE_DIR}/lib)
message(POD5_LIBRARY_DIR="${POD5_LIBRARY_DIR}")

find_package(GSL REQUIRED)

add_subdirectory(src)
add_subdirectory(test)