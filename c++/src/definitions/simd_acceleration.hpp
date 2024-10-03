#pragma once

enum SIMDAcceleration
{
    None = 0,
    AVX2 = 1,
    SSE = 2,
    RuntimeDefined = 3
};