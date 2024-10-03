#pragma once

#ifdef _MSC_VER
#define parallel_nanopore_restrict __restrict
#else
#define parallel_nanopore_restrict __restrict__
#endif

#define ZSTDImplTemplateDeclarationHeaderV1 \
    size_t (*ZSTD_compressBound_impl) (size_t),\
    size_t (*ZSTD_compress_impl) (void * dst, size_t dstCapacity, const void* src, size_t srcSize, int compressionLevel),\
    size_t (*ZSTD_decompress_impl) (void* dst, size_t dstCapacity, const void* src, size_t srcSize),\
    unsigned (*ZSTD_isError_impl) (size_t)

#define ZSTDImplTemplateDeclarationHeader\
    size_t (*ZSTD_compressBound_impl) (size_t),\
    size_t (*ZSTD_compress_impl) (void * dst, size_t dstCapacity, const void* src, size_t srcSize, int compressionLevel),\
    size_t (*ZSTD_decompress_impl) (void* dst, size_t dstCapacity, const void* src, size_t srcSize),\
    unsigned (*ZSTD_isError_impl) (size_t), \
    unsigned long long (*ZSTD_getFrameContentSize_impl) (const void* src, size_t srcSize)

#define ZSTDImplTemplateParameterHeaderV1 \
    ZSTD_compressBound_impl, \
    ZSTD_compress_impl,\
    ZSTD_decompress_impl,\
    ZSTD_isError_impl

#define ZSTDImplTemplateParameterHeader \
    ZSTD_compressBound_impl, \
    ZSTD_compress_impl,\
    ZSTD_decompress_impl,\
    ZSTD_isError_impl, \
    ZSTD_getFrameContentSize_impl

#define RealZSTDProvider\
    ZSTD_compressBound, ZSTD_compress, ZSTD_decompress, ZSTD_isError, ZSTD_getFrameContentSize

#define MockZSTDProvider\
    ZSTD_compressBound_mock, ZSTD_compress_mock, ZSTD_decompress_mock, ZSTD_isError_mock, ZSTD_getFrameContentSize_mock

#define OUT 