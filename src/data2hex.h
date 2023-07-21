#pragma once
#include <intrin.h>

inline void sse2_data2hex(const void* src, char* dest, int size) {
    const __m128i* srcptr = (const __m128i*)src;
    __m128i* destptr = (__m128i*)dest;
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
    int i;
    short* destptr2;
    dest[size * 2] = 0; // add termination of string
    for (i = 0; i < size / 16; i++)
    {
        srcw = _mm_loadu_si128(srcptr);
        low = _mm_unpacklo_epi8(srcw, srcw); // expand byte to word
        
        temp = _mm_srai_epi16(low, 4); // separate high digit and low digit
        temp = _mm_and_si128(temp, maskhi);
        low = _mm_and_si128(low, masklo);
        low = _mm_or_si128(low, temp);

        cmp_inv = _mm_cmpgt_epi8(low, nines); // compare, create mask
        cmp = _mm_andnot_si128(cmp_inv, ones);
        cmp = _mm_and_si128(cmp, zeros);
        cmp_inv = _mm_and_si128(cmp_inv, as);
        cmp = _mm_or_si128(cmp, cmp_inv);
        low = _mm_adds_epi8(low, cmp); // add base character

        // repeat for high
        high = _mm_unpackhi_epi8(srcw, srcw);
        temp = _mm_srai_epi16(high, 4);
        temp = _mm_and_si128(temp, maskhi);
        high = _mm_and_si128(high, masklo);
        high = _mm_or_si128(high, temp);

        cmp_inv = _mm_cmpgt_epi8(high, nines);
        cmp = _mm_andnot_si128(cmp_inv, ones);
        cmp = _mm_and_si128(cmp, zeros);
        cmp_inv = _mm_and_si128(cmp_inv, as);
        cmp = _mm_or_si128(cmp, cmp_inv);
        high = _mm_adds_epi8(high, cmp);

        _mm_storeu_si128(destptr, low);
        _mm_storeu_si128(destptr + 1, high);
        destptr += 2;
        srcptr += 1;
    }
    i = size - i * 16;
    if (i == 0)
        return;
    // last bytes
    srcw = _mm_loadu_si128(srcptr);
    low = _mm_unpacklo_epi8(srcw, srcw); // expand byte to word

    temp = _mm_srai_epi16(low, 4); // separate high digit and low digit
    temp = _mm_and_si128(temp, maskhi);
    low = _mm_and_si128(low, masklo);
    low = _mm_or_si128(low, temp);

    cmp_inv = _mm_cmpgt_epi8(low, nines); // compare, create mask
    cmp = _mm_andnot_si128(cmp_inv, ones);
    cmp = _mm_and_si128(cmp, zeros);
    cmp_inv = _mm_and_si128(cmp_inv, as);
    cmp = _mm_or_si128(cmp, cmp_inv);
    low = _mm_adds_epi8(low, cmp); // add base character

    if (i < 8)
    {
        destptr2 = (short*)destptr;
        if (i >= 4)
        {
            _mm_storel_epi64(destptr, low); //4
            if (i > 4)
                destptr2[4] = _mm_extract_epi16(low, 4); //5
            if (i > 5)
                destptr2[5] = _mm_extract_epi16(low, 5); //6
            if (i > 6)
                destptr2[6] = _mm_extract_epi16(low, 6); //7
        }
        else
        {
            destptr2[0] = _mm_extract_epi16(low, 0); //1
            if(i>1)
                destptr2[1] = _mm_extract_epi16(low, 1); //2
            if (i > 2)
                destptr2[2] = _mm_extract_epi16(low, 2); //3
        }
        return;
    }
    _mm_storeu_si128(destptr, low);
    if (i == 8)
        return;
    destptr++;
    i -= 8;
    // repeat for high
    high = _mm_unpackhi_epi8(srcw, srcw);
    temp = _mm_srai_epi16(high, 4);
    temp = _mm_and_si128(temp, maskhi);
    high = _mm_and_si128(high, masklo);
    high = _mm_or_si128(high, temp);

    cmp_inv = _mm_cmpgt_epi8(high, nines);
    cmp = _mm_andnot_si128(cmp_inv, ones);
    cmp = _mm_and_si128(cmp, zeros);
    cmp_inv = _mm_and_si128(cmp_inv, as);
    cmp = _mm_or_si128(cmp, cmp_inv);
    high = _mm_adds_epi8(high, cmp);
    destptr2 = (short*)destptr;
    if (i >= 4)
    {
        _mm_storel_epi64(destptr, high); //4
        if (i > 4)
            destptr2[4] = _mm_extract_epi16(high, 4); //5
        if (i > 5)
            destptr2[5] = _mm_extract_epi16(high, 5); //6
        if (i > 6)
            destptr2[6] = _mm_extract_epi16(high, 6); //7
    }
    else
    {
        destptr2[0] = _mm_extract_epi16(high, 0); //1
        if (i > 1)
            destptr2[1] = _mm_extract_epi16(high, 1); //2
        if (i > 2)
            destptr2[2] = _mm_extract_epi16(high, 2); //3
    }
}

inline void avx2_data2hex(const void* src, char* dest, int size) {
    const __m128i* srcptr = (const __m128i*)src;
    // use 'a' for lower case output, 'A' for upper case output
    char A = 'a' - 10;
    __m256i* destptr256 = (__m256i*)dest;
    __m128i nines = _mm_set1_epi8(9);
    __m128i ones = _mm_set1_epi8(0xFF);
    __m128i masklo = _mm_setr_epi8(0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF);
    __m128i maskhi = _mm_setr_epi8(0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0, 0xF, 0);
    __m128i zeros = _mm_set1_epi8('0');
    __m128i as = _mm_set1_epi8(A);

    __m256i nines2 = _mm256_broadcastsi128_si256(nines);
    __m256i ones2 = _mm256_broadcastsi128_si256(ones);
    __m256i masklo2 = _mm256_broadcastsi128_si256(masklo);
    __m256i maskhi2 = _mm256_broadcastsi128_si256(maskhi);
    __m256i zeros2 = _mm256_broadcastsi128_si256(zeros);
    __m256i as2 = _mm256_broadcastsi128_si256(as);
    __m128i srcw;
    __m256i srcw1;
    __m256i low2;
    __m256i temp2;
    __m256i cmp_inv2;
    __m256i cmp2;
    int i;
    short* destptr2;
    dest[size * 2] = 0; // add termination of string
    for (i = 0; i < size / 16; i++)
    {
        srcw = _mm_loadu_si128(srcptr);
        srcw1 = _mm256_castsi128_si256(srcw);
        low2 = _mm256_unpacklo_epi8(srcw1, srcw1); // expand byte to word
        low2 = _mm256_insertf128_si256(low2, _mm_unpackhi_epi8(srcw, srcw), 1);

        temp2 = _mm256_srai_epi16(low2, 4); // separate high digit and low digit
        temp2 = _mm256_and_si256(temp2, maskhi2);
        low2 = _mm256_and_si256(low2, masklo2);
        low2 = _mm256_or_si256(low2, temp2);

        cmp_inv2 = _mm256_cmpgt_epi8(low2, nines2); // compare, create mask
        cmp2 = _mm256_andnot_si256(cmp_inv2, ones2);
        cmp2 = _mm256_and_si256(cmp2, zeros2);
        cmp_inv2 = _mm256_and_si256(cmp_inv2, as2);
        cmp2 = _mm256_or_si256(cmp2, cmp_inv2);
        low2 = _mm256_adds_epi8(low2, cmp2); // add base character

        _mm256_storeu_si256(destptr256, low2);
        destptr256++;
        srcptr += 1;
    }
    i = size - i * 16;
    if (i == 0)
        return;
    // last bytes
    srcw = _mm_loadu_si128(srcptr);
    srcw1 = _mm256_castsi128_si256(srcw);
    low2 = _mm256_unpacklo_epi8(srcw1, srcw1); // expand byte to word
    low2 = _mm256_insertf128_si256(low2, _mm_unpackhi_epi8(srcw, srcw), 1);

    temp2 = _mm256_srai_epi16(low2, 4); // separate high digit and low digit
    temp2 = _mm256_and_si256(temp2, maskhi2);
    low2 = _mm256_and_si256(low2, masklo2);
    low2 = _mm256_or_si256(low2, temp2);

    cmp_inv2 = _mm256_cmpgt_epi8(low2, nines2); // compare, create mask
    cmp2 = _mm256_andnot_si256(cmp_inv2, ones2);
    cmp2 = _mm256_and_si256(cmp2, zeros2);
    cmp_inv2 = _mm256_and_si256(cmp_inv2, as2);
    cmp2 = _mm256_or_si256(cmp2, cmp_inv2);
    low2 = _mm256_adds_epi8(low2, cmp2); // add base character

    __m128i low = _mm256_castsi256_si128(low2);
    __m128i high = _mm256_extracti128_si256(low2, 1);
    /*__m128i mask = _mm_setr_epi16(0, 1, 2, 3, 4, 5, 6, 7); //this is slow
    if (i <= 8)
    {
        mask = _mm_cmplt_epi16(mask, _mm_set1_epi16(i));
        _mm_maskmoveu_si128(low, mask, destptr256);
    }
    else
    {
        _mm_storeu_si128(destptr256, low);
        mask = _mm_cmplt_epi16(mask, _mm_set1_epi16(i-8));
        _mm_maskmoveu_si128(low, mask, ((char*)destptr256)+16);
    }*/

    __m128i* destptr = (__m128i*)destptr256;
    if (i < 8)
    {
        destptr2 = (short*)destptr;
        if (i >= 4)
        {
            _mm_storel_epi64(destptr, low); //4
            if (i > 4)
                destptr2[4] = _mm_extract_epi16(low, 4); //5
            if (i > 5)
                destptr2[5] = _mm_extract_epi16(low, 5); //6
            if (i > 6)
                destptr2[6] = _mm_extract_epi16(low, 6); //7
        }
        else
        {
            destptr2[0] = _mm_extract_epi16(low, 0); //1
            if (i > 1)
                destptr2[1] = _mm_extract_epi16(low, 1); //2
            if (i > 2)
                destptr2[2] = _mm_extract_epi16(low, 2); //3
        }
        return;
    }
    _mm_storeu_si128(destptr, low);
    if (i == 8)
        return;
    destptr++;
    i -= 8;
    destptr2 = (short*)destptr;
    if (i >= 4)
    {
        _mm_storel_epi64(destptr, high); //4
        if (i > 4)
            destptr2[4] = _mm_extract_epi16(high, 4); //5
        if (i > 5)
            destptr2[5] = _mm_extract_epi16(high, 5); //6
        if (i > 6)
            destptr2[6] = _mm_extract_epi16(high, 6); //7
    }
    else
    {
        destptr2[0] = _mm_extract_epi16(high, 0); //1
        if (i > 1)
            destptr2[1] = _mm_extract_epi16(high, 1); //2
        if (i > 2)
            destptr2[2] = _mm_extract_epi16(high, 2); //3
    }
}
