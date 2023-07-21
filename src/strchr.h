#pragma once
#include <intrin.h>

inline const char* sse42_strchr(const char* a, char b)
{
    const __m128i* ptr = (const __m128i*)a;
    __m128i find = _mm_set1_epi8(b);
    while (1) {
        // these _mm_cmpistrc should emit only 1 PCMPESTRI if possible
        // break when found
        if (_mm_cmpistrc(find, *ptr, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT))
            break;
        // break at string end
        if (_mm_cmpistrz(find, *ptr, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT))
            return NULL;
        ptr++;
    }
    // shouldn't emit any more PCMPESTRI
    // if found, return ptr
    const char* p = (const char*)ptr;
    p += _mm_cmpistri(find, *ptr, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT);
    return p;
}
