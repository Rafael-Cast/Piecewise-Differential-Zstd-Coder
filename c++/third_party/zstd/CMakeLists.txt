include(FetchContent)

set(ZSTD_BUILD_STATIC ON)
set(ZSTD_BUILD_SHARED OFF)
set(CMAKE_BUILD_TYPE Release)

FetchContent_Declare(
    zstd
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SOURCE_SUBDIR build/cmake
    URL "https://github.com/facebook/zstd/releases/download/v1.5.5/zstd-1.5.5.tar.gz"
)

FetchContent_MakeAvailable(zstd)

link_libraries(
    PRIVATE
    libzstd_static
)

# On windows and macos this is needed
include_directories(
    PRIVATE
    ${zstd_SOURCE_DIR}/lib
)