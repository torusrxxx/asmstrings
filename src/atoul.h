#pragma once
#include <intrin.h>

inline unsigned long sse42_atoul(const char* str)
{
    __m128i limits = _mm_setr_epi8('0', '9', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    __m128i zeros = _mm_set1_epi8('0');
    __m128i d = _mm_loadu_si128((const __m128i*)str);
    // get characters within 0 to 9
    __m128i check = _mm_cmpistrm(limits, d, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_UNIT_MASK);
    // reverse the order
    int len = _mm_cmpistri(limits, d, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_MOST_SIGNIFICANT);
    __m128i digits = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
    digits = _mm_subs_epi8(digits, _mm_set1_epi8(15 - len));
    d = _mm_shuffle_epi8(d, digits); //reversed order, and set unused parts to 0
    check = _mm_and_si128(check, zeros);
    d = _mm_sub_epi8(d, check);
    d = _mm_maddubs_epi16(d, _mm_set1_epi16(0x0A01));
    d = _mm_madd_epi16(d, _mm_set1_epi32(0x00640001));
    d = _mm_mullo_epi32(d, _mm_setr_epi32(1, 10000, 100000000, 0));
    d = _mm_hadd_epi32(d, d);
    d = _mm_hadd_epi32(d, d);
    return _mm_extract_epi32(d, 0);
}
