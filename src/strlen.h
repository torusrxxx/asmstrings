#pragma once
#include <intrin.h>

inline unsigned int avx2_strlen(const char* str) {
    __m256i zeros = _mm256_setzero_si256();
    __m256i *ptr = (__m256i*)str;
    unsigned int result = 0;
    unsigned int ecx;
    do {
        __m256i src = _mm256_loadu_si256(ptr);
        __m256i r = _mm256_cmpeq_epi8(zeros, src);
        ecx = _mm256_movemask_epi8(r);
        ecx = _tzcnt_u32(ecx);
        result += ecx;
        ptr++;
    } while (ecx == 32);
    return result;
}

inline unsigned int sse42_strlen(const char* str) {
    __m128i zeros = _mm_setzero_si128();
    __m128i *ptr = (__m128i*)str;
    unsigned int result = 0;
    unsigned int ecx;
    while (!_mm_cmpistrz(zeros, *ptr, _SIDD_UBYTE_OPS))
    {
        ptr++;
    }
    __m128i src = _mm_loadu_si128(ptr);
    __m128i r = _mm_cmpeq_epi8(zeros, src);
    ecx = _mm_movemask_epi8(r);
    ecx = _tzcnt_u32(ecx);
    result = (((char*)ptr) - str) + ecx;
    return result;
}
