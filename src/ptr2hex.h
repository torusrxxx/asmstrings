#pragma once
#include <intrin.h>

// pointer to string with leading zeros
inline void sse42_ptr2hex(uintptr_t ptr, char* dest) {
    __m128i data = _mm_setzero_si128();
    // load data and swap bytes
    if (sizeof(void*) == 8)
        data = _mm_insert_epi64(data, _byteswap_uint64(ptr), 0);
    else if (sizeof(void*) == 4)
        data = _mm_insert_epi32(data, _byteswap_ulong(ptr), 0);
    dest[sizeof(void*) * 2] = '\0';
    // from data2hex,h
    __m128i nines = _mm_set1_epi8(9);
    __m128i ones = _mm_set1_epi8(0xFF);
    __m128i masklo = _mm_setr_epi8(0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF);
    __m128i maskhi = _mm_setr_epi8(0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0);
    __m128i zeros = _mm_set1_epi8('0');
    // use 'a' for lower case output, 'A' for upper case output
    char A = 'A' - 10;
    __m128i as = _mm_set1_epi8(A);
    __m128i srcw;
    __m128i low;
    __m128i high;
    __m128i temp;
    __m128i cmp_inv;
    __m128i cmp;
    low = _mm_unpacklo_epi8(data, data); // expand byte to word

    temp = _mm_srai_epi16(low, 4); // separate high digit and low digit
    temp = _mm_and_si128(temp, maskhi);
    low = _mm_and_si128(low, masklo);
    low = _mm_or_si128(low, temp);

    cmp_inv = _mm_cmpgt_epi8(low, nines); // compare, create mask
    cmp = _mm_andnot_si128(cmp_inv, ones);
    cmp = _mm_and_si128(cmp, zeros);
    cmp_inv = _mm_and_si128(cmp_inv, as);
    cmp = _mm_or_si128(cmp, cmp_inv);
    //cmp = _mm_blendv_epi8(zeros, as, cmp_inv); // this is slower
    low = _mm_adds_epi8(low, cmp); // add base character
    if (sizeof(void*) == 8)
        _mm_storeu_si128((__m128i*)dest, low);
    else if (sizeof(void*) == 4)
        _mm_storel_epi64((__m128i*)dest, low);
}
