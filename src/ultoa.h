#pragma once
#include <intrin.h>

void sse42_ultoa(unsigned long value, char* buffer) {
    unsigned int parts[2];
    parts[1] = value / 100000000;
    parts[0] = value - parts[1] * 100000000;
    __m128i a = _mm_loadl_epi64((const __m128i*)parts);
    // div10000 = a / 10000
    a = _mm_shuffle_epi32(a, 0xDC);
    __m128i div10000 = _mm_mul_epu32(a, _mm_set1_epi32(0xd1b71759));
    div10000 = _mm_srli_epi64(div10000, 13);
    // mask off low part, leave high part
    div10000 = _mm_and_si128(div10000, _mm_setr_epi32(0, -1, 0, -1));
    // mov10000 = a % 10000 = a - div10000 * 10000
    __m128i mod10000 = _mm_set1_epi32(10000);
    mod10000 = _mm_mul_epu32(mod10000, _mm_shuffle_epi32(div10000, 0x21));
    mod10000 = _mm_sub_epi32(a, mod10000);
    // pack, now 4 int32 with 4 digits each 0-9999
    __m128i digits = _mm_or_si128(mod10000, div10000);
    __m128i div100 = _mm_mulhi_epu16(digits, _mm_set1_epi16(0xa3d8));
    div100 = _mm_srli_epi16(div100, 6);
    __m128i mod100 = _mm_mullo_epi16(div100, _mm_set1_epi16(100));
    mod100 = _mm_sub_epi16(digits, mod100);
    div100 = _mm_slli_epi32(div100, 16);
    // now 8 int16 with 2 digits each 0-99
    digits = _mm_or_si128(div100, mod100);
    __m128i div10 = _mm_mullo_epi16(digits, _mm_set1_epi16(103));
    div10 = _mm_srli_epi16(div10, 10);
    __m128i mod10 = _mm_mullo_epi16(div10, _mm_set1_epi16(10));
    mod10 = _mm_sub_epi8(digits, mod10);
    div10 = _mm_slli_epi32(div10, 8);
    // now unpacked BCD representation
    digits = _mm_or_si128(div10, mod10);
    digits = _mm_add_epi8(digits, _mm_set1_epi8('0'));
    // reverse the order
    int len = _mm_cmpistri(_mm_setr_epi8('1', '9', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0), digits, _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_MOST_SIGNIFICANT);
    __m128i reverse_mask = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
    reverse_mask = _mm_subs_epi8(reverse_mask, _mm_set1_epi8(15 - len));
    digits = _mm_shuffle_epi8(digits, reverse_mask); //reversed order, and set unused parts to 0
    _mm_storeu_si128((__m128i*)buffer, digits);
}
