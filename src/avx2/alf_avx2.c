﻿/**************************************************************************************
 * Copyright (C) 2018-2019 uavs3e project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Open-Intelligence Open Source License V1.1.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Open-Intelligence Open Source License V1.1 for more details.
 *
 * You should have received a copy of the Open-Intelligence Open Source License V1.1
 * along with this program; if not, you can download it on:
 * http://www.aitisa.org.cn/uploadfile/2018/0910/20180910031548314.pdf
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#include "avx2.h"

#if (BIT_DEPTH == 8)

void uavs3e_alf_one_lcu_avx2(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int sample_bit_depth)
{
    pel *imgPad1, *imgPad2, *imgPad3, *imgPad4, *imgPad5, *imgPad6;

    __m256i T00, T01, T10, T11, T20, T21, T30, T31, T40, T41, T50, T51, T60, T61, T70, T71, T80, T81, T82, T83;
    __m256i T8;
    __m256i T000, T001, T100, T101, T200, T201, T300, T301, T400, T401, T500, T501, T600, T601, T700, T701;
    __m256i C0, C1, C2, C3, C4, C5, C6, C7, C8;
    __m256i S0, S00, S01, S1, S10, S11, S2, S20, S21, S3, S30, S31, S4, S40, S41, S5, S50, S51, S6, S7, S60, S61, S70, S71, SS1, SS2, SS3, SS4, S;
    __m256i mAddOffset;
    __m256i mZero = _mm256_set1_epi16(0);
    __m256i mMax = _mm256_set1_epi16((short)((1 << sample_bit_depth) - 1));
    __m128i m0, m1;

    int i, j;
    int startPos = 0;
    int endPos = lcu_height;

    C0 = _mm256_set1_epi8(coef[0]);     // C0-C7: [-64, 63]
    C1 = _mm256_set1_epi8(coef[1]);
    C2 = _mm256_set1_epi8(coef[2]);
    C3 = _mm256_set1_epi8(coef[3]);
    C4 = _mm256_set1_epi8(coef[4]);
    C5 = _mm256_set1_epi8(coef[5]);
    C6 = _mm256_set1_epi8(coef[6]);
    C7 = _mm256_set1_epi8(coef[7]);
    C8 = _mm256_set1_epi32(coef[8]);    // [-1088, 1071]

    mAddOffset = _mm256_set1_epi32(32);

    for (i = startPos; i < endPos; i++) {
        imgPad1 = src + i_src;
        imgPad2 = src - i_src;
        imgPad3 = src + 2 * i_src;
        imgPad4 = src - 2 * i_src;
        imgPad5 = src + 3 * i_src;
        imgPad6 = src - 3 * i_src;
        if (i < 3) {
            if (i == 0) {
                imgPad4 = imgPad2 = src;
            } else if (i == 1) {
                imgPad4 = imgPad2;
            }
            imgPad6 = imgPad4;
        } else if (i > lcu_height - 4) {
            if (i == lcu_height - 1) {
                imgPad3 = imgPad1 = src;
            } else if (i == lcu_height - 2) {
                imgPad3 = imgPad1;
            }
            imgPad5 = imgPad3;
        }

        for (j = 0; j < lcu_width; j += 32) {
            T00 = _mm256_loadu_si256((__m256i *)&imgPad6[j]);
            T01 = _mm256_loadu_si256((__m256i *)&imgPad5[j]);
            T000 = _mm256_unpacklo_epi8(T00, T01);
            T001 = _mm256_unpackhi_epi8(T00, T01);
            S00 = _mm256_maddubs_epi16(T000, C0);
            S01 = _mm256_maddubs_epi16(T001, C0);

            T10 = _mm256_loadu_si256((__m256i *)&imgPad4[j]);
            T11 = _mm256_loadu_si256((__m256i *)&imgPad3[j]);
            T100 = _mm256_unpacklo_epi8(T10, T11);
            T101 = _mm256_unpackhi_epi8(T10, T11);
            S10 = _mm256_maddubs_epi16(T100, C1);
            S11 = _mm256_maddubs_epi16(T101, C1);

            T20 = _mm256_loadu_si256((__m256i *)&imgPad2[j - 1]);
            T30 = _mm256_loadu_si256((__m256i *)&imgPad2[j]);
            T40 = _mm256_loadu_si256((__m256i *)&imgPad2[j + 1]);
            T41 = _mm256_loadu_si256((__m256i *)&imgPad1[j - 1]);
            T31 = _mm256_loadu_si256((__m256i *)&imgPad1[j]);
            T21 = _mm256_loadu_si256((__m256i *)&imgPad1[j + 1]);

            T200 = _mm256_unpacklo_epi8(T20, T21);
            T201 = _mm256_unpackhi_epi8(T20, T21);
            T300 = _mm256_unpacklo_epi8(T30, T31);
            T301 = _mm256_unpackhi_epi8(T30, T31);
            T400 = _mm256_unpacklo_epi8(T40, T41);
            T401 = _mm256_unpackhi_epi8(T40, T41);
            S20 = _mm256_maddubs_epi16(T200, C2);
            S21 = _mm256_maddubs_epi16(T201, C2);
            S30 = _mm256_maddubs_epi16(T300, C3);
            S31 = _mm256_maddubs_epi16(T301, C3);
            S40 = _mm256_maddubs_epi16(T400, C4);
            S41 = _mm256_maddubs_epi16(T401, C4);

            T50 = _mm256_loadu_si256((__m256i *)&src[j - 3]);
            T60 = _mm256_loadu_si256((__m256i *)&src[j - 2]);
            T70 = _mm256_loadu_si256((__m256i *)&src[j - 1]);
            T8 = _mm256_loadu_si256((__m256i *)&src[j]);
            T71 = _mm256_loadu_si256((__m256i *)&src[j + 1]);
            T61 = _mm256_loadu_si256((__m256i *)&src[j + 2]);
            T51 = _mm256_loadu_si256((__m256i *)&src[j + 3]);

            m0 = _mm256_castsi256_si128(T8);
            m1 = _mm256_extracti128_si256(T8, 1);

            T80 = _mm256_cvtepu8_epi32(m0);
            T81 = _mm256_cvtepu8_epi32(_mm_srli_si128(m0, 8));
            T82 = _mm256_cvtepu8_epi32(m1);
            T83 = _mm256_cvtepu8_epi32(_mm_srli_si128(m1, 8));
            T80 = _mm256_mullo_epi32(T80, C8);
            T81 = _mm256_mullo_epi32(T81, C8);
            T82 = _mm256_mullo_epi32(T82, C8);
            T83 = _mm256_mullo_epi32(T83, C8);

            T500 = _mm256_unpacklo_epi8(T50, T51);
            T501 = _mm256_unpackhi_epi8(T50, T51);
            T600 = _mm256_unpacklo_epi8(T60, T61);
            T601 = _mm256_unpackhi_epi8(T60, T61);
            T700 = _mm256_unpacklo_epi8(T70, T71);
            T701 = _mm256_unpackhi_epi8(T70, T71);
            S50 = _mm256_maddubs_epi16(T500, C5);
            S51 = _mm256_maddubs_epi16(T501, C5);
            S60 = _mm256_maddubs_epi16(T600, C6);
            S61 = _mm256_maddubs_epi16(T601, C6);
            S70 = _mm256_maddubs_epi16(T700, C7);
            S71 = _mm256_maddubs_epi16(T701, C7);

            S0 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S00));
            S1 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S10));
            S2 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S20));
            S3 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S30));
            S4 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S40));
            S5 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S50));
            S6 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S60));
            S7 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S70));
            S0 = _mm256_add_epi32(S0, S1);
            S2 = _mm256_add_epi32(S2, S3);
            S4 = _mm256_add_epi32(S4, S5);
            S6 = _mm256_add_epi32(S6, S7);
            S0 = _mm256_add_epi32(S0, S2);
            S4 = _mm256_add_epi32(S4, S6);
            SS1 = _mm256_add_epi32(S0, S4);
            SS1 = _mm256_add_epi32(SS1, T80);    //0-7

            S0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S00, 1));
            S1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S10, 1));
            S2 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S20, 1));
            S3 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S30, 1));
            S4 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S40, 1));
            S5 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S50, 1));
            S6 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S60, 1));
            S7 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S70, 1));
            S0 = _mm256_add_epi32(S0, S1);
            S2 = _mm256_add_epi32(S2, S3);
            S4 = _mm256_add_epi32(S4, S5);
            S6 = _mm256_add_epi32(S6, S7);
            S0 = _mm256_add_epi32(S0, S2);
            S4 = _mm256_add_epi32(S4, S6);
            SS2 = _mm256_add_epi32(S0, S4);
            SS2 = _mm256_add_epi32(SS2, T82);    //16-23

            S0 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S01));
            S1 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S11));
            S2 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S21));
            S3 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S31));
            S4 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S41));
            S5 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S51));
            S6 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S61));
            S7 = _mm256_cvtepi16_epi32(_mm256_castsi256_si128(S71));
            S0 = _mm256_add_epi32(S0, S1);
            S2 = _mm256_add_epi32(S2, S3);
            S4 = _mm256_add_epi32(S4, S5);
            S6 = _mm256_add_epi32(S6, S7);
            S0 = _mm256_add_epi32(S0, S2);
            S4 = _mm256_add_epi32(S4, S6);
            SS3 = _mm256_add_epi32(S0, S4);
            SS3 = _mm256_add_epi32(SS3, T81);    //8-15

            S0 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S01, 1));
            S1 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S11, 1));
            S2 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S21, 1));
            S3 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S31, 1));
            S4 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S41, 1));
            S5 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S51, 1));
            S6 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S61, 1));
            S7 = _mm256_cvtepi16_epi32(_mm256_extracti128_si256(S71, 1));
            S0 = _mm256_add_epi32(S0, S1);
            S2 = _mm256_add_epi32(S2, S3);
            S4 = _mm256_add_epi32(S4, S5);
            S6 = _mm256_add_epi32(S6, S7);
            S0 = _mm256_add_epi32(S0, S2);
            S4 = _mm256_add_epi32(S4, S6);
            SS4 = _mm256_add_epi32(S0, S4);
            SS4 = _mm256_add_epi32(SS4, T83);    //24-31

            SS1 = _mm256_add_epi32(SS1, mAddOffset);
            SS2 = _mm256_add_epi32(SS2, mAddOffset);
            SS3 = _mm256_add_epi32(SS3, mAddOffset);
            SS4 = _mm256_add_epi32(SS4, mAddOffset);

            SS1 = _mm256_srai_epi32(SS1, 6);
            SS2 = _mm256_srai_epi32(SS2, 6);
            SS3 = _mm256_srai_epi32(SS3, 6);
            SS4 = _mm256_srai_epi32(SS4, 6);

            SS1 = _mm256_packs_epi32(SS1, SS2);
            SS3 = _mm256_packs_epi32(SS3, SS4);
            SS1 = _mm256_permute4x64_epi64(SS1, 0xd8);
            SS3 = _mm256_permute4x64_epi64(SS3, 0xd8);

            SS1 = _mm256_min_epi16(SS1, mMax);
            SS1 = _mm256_max_epi16(SS1, mZero);
            SS3 = _mm256_min_epi16(SS3, mMax);
            SS3 = _mm256_max_epi16(SS3, mZero);

            S = _mm256_packus_epi16(SS1, SS3);
            _mm256_storeu_si256((__m256i *)(dst + j), S);
        }

        src += i_src;
        dst += i_dst;
    }
}

#else

void uavs3e_alf_one_lcu_avx2(pel *dst, int i_dst, pel *src, int i_src, int lcu_width, int lcu_height, int *coef, int bit_depth)
{
    pel *imgPad1, *imgPad2, *imgPad3, *imgPad4, *imgPad5, *imgPad6;

    __m256i T00, T01, T10, T11, T20, T21, T30, T31, T40, T41, T42, T43, T44, T45;
    __m256i E00, E01, E10, E11, E20, E21, E30, E31, E40, E41, E42, E43, E44, E45;
    __m256i C0, C1, C2, C3, C4, C5, C6, C7, C8;
    __m256i S00, S01, S10, S11, S20, S21, S30, S31, S40, S41, S50, S51, S60, S61, SS1, SS2, S, S70, S71, S80, S81;
    __m256i mAddOffset;
    __m256i zero = _mm256_setzero_si256();
    int max_pixel = (1 << bit_depth) - 1;
    __m256i max_val = _mm256_set1_epi16(max_pixel);

    int i, j;
    int startPos = 0;
    int endPos = lcu_height;

    C0 = _mm256_set1_epi16((pel)coef[0]);
    C1 = _mm256_set1_epi16((pel)coef[1]);
    C2 = _mm256_set1_epi16((pel)coef[2]);
    C3 = _mm256_set1_epi16((pel)coef[3]);
    C4 = _mm256_set1_epi16((pel)coef[4]);
    C5 = _mm256_set1_epi16((pel)coef[5]);
    C6 = _mm256_set1_epi16((pel)coef[6]);
    C7 = _mm256_set1_epi16((pel)coef[7]);
    C8 = _mm256_set1_epi16((pel)coef[8]);

    mAddOffset = _mm256_set1_epi32(32);

    for (i = 0; i < lcu_height; i++) {
        imgPad1 = src + i_src;
        imgPad2 = src - i_src;
        imgPad3 = src + 2 * i_src;
        imgPad4 = src - 2 * i_src;
        imgPad5 = src + 3 * i_src;
        imgPad6 = src - 3 * i_src;
        if (i < 3) {
            if (i == 0) {
                imgPad4 = imgPad2 = src;
            } else if (i == 1) {
                imgPad4 = imgPad2;
            }
            imgPad6 = imgPad4;
        } else if (i > lcu_height - 4) {
            if (i == lcu_height - 1) {
                imgPad3 = imgPad1 = src;
            } else if (i == lcu_height - 2) {
                imgPad3 = imgPad1;
            }
            imgPad5 = imgPad3;
        }

        for (j = 0; j < lcu_width; j += 16) {
            T00 = _mm256_loadu_si256((__m256i *)&imgPad6[j]);
            T01 = _mm256_loadu_si256((__m256i *)&imgPad5[j]);
            T10 = _mm256_loadu_si256((__m256i *)&imgPad4[j]);
            T11 = _mm256_loadu_si256((__m256i *)&imgPad3[j]);
            E00 = _mm256_unpacklo_epi16(T00, T01);
            E01 = _mm256_unpackhi_epi16(T00, T01);
            E10 = _mm256_unpacklo_epi16(T10, T11);
            E11 = _mm256_unpackhi_epi16(T10, T11);

            S00 = _mm256_madd_epi16(E00, C0);//前8个像素所有C0*P0的结果
            S01 = _mm256_madd_epi16(E01, C0);//后8个像素所有C0*P0的结果
            S10 = _mm256_madd_epi16(E10, C1);//前8个像素所有C1*P1的结果
            S11 = _mm256_madd_epi16(E11, C1);//后8个像素所有C1*P1的结果

            T20 = _mm256_loadu_si256((__m256i *)&imgPad2[j - 1]);
            T21 = _mm256_loadu_si256((__m256i *)&imgPad1[j + 1]);
            T30 = _mm256_loadu_si256((__m256i *)&imgPad2[j]);
            T31 = _mm256_loadu_si256((__m256i *)&imgPad1[j]);
            T40 = _mm256_loadu_si256((__m256i *)&imgPad2[j + 1]);
            T41 = _mm256_loadu_si256((__m256i *)&imgPad1[j - 1]);

            E20 = _mm256_unpacklo_epi16(T20, T21);
            E21 = _mm256_unpackhi_epi16(T20, T21);
            E30 = _mm256_unpacklo_epi16(T30, T31);
            E31 = _mm256_unpackhi_epi16(T30, T31);
            E40 = _mm256_unpacklo_epi16(T40, T41);
            E41 = _mm256_unpackhi_epi16(T40, T41);

            S20 = _mm256_madd_epi16(E20, C2);
            S21 = _mm256_madd_epi16(E21, C2);
            S30 = _mm256_madd_epi16(E30, C3);
            S31 = _mm256_madd_epi16(E31, C3);
            S40 = _mm256_madd_epi16(E40, C4);
            S41 = _mm256_madd_epi16(E41, C4);

            T40 = _mm256_loadu_si256((__m256i *)&src[j - 3]);
            T41 = _mm256_loadu_si256((__m256i *)&src[j + 3]);
            T42 = _mm256_loadu_si256((__m256i *)&src[j - 2]);
            T43 = _mm256_loadu_si256((__m256i *)&src[j + 2]);
            T44 = _mm256_loadu_si256((__m256i *)&src[j - 1]);
            T45 = _mm256_loadu_si256((__m256i *)&src[j + 1]);

            E40 = _mm256_unpacklo_epi16(T40, T41);
            E41 = _mm256_unpackhi_epi16(T40, T41);
            E42 = _mm256_unpacklo_epi16(T42, T43);
            E43 = _mm256_unpackhi_epi16(T42, T43);
            E44 = _mm256_unpacklo_epi16(T44, T45);
            E45 = _mm256_unpackhi_epi16(T44, T45);

            S50 = _mm256_madd_epi16(E40, C5);
            S51 = _mm256_madd_epi16(E41, C5);
            S60 = _mm256_madd_epi16(E42, C6);
            S61 = _mm256_madd_epi16(E43, C6);
            S70 = _mm256_madd_epi16(E44, C7);
            S71 = _mm256_madd_epi16(E45, C7);

            T40 = _mm256_loadu_si256((__m256i *)&src[j]);
            E40 = _mm256_unpacklo_epi16(T40, zero);
            E41 = _mm256_unpackhi_epi16(T40, zero);
            S80 = _mm256_madd_epi16(E40, C8);
            S81 = _mm256_madd_epi16(E41, C8);

            SS1 = _mm256_add_epi32(S00, S10);
            SS1 = _mm256_add_epi32(SS1, S20);
            SS1 = _mm256_add_epi32(SS1, S30);
            SS1 = _mm256_add_epi32(SS1, S40);
            SS1 = _mm256_add_epi32(SS1, S50);
            SS1 = _mm256_add_epi32(SS1, S60);
            SS1 = _mm256_add_epi32(SS1, S70);
            SS1 = _mm256_add_epi32(SS1, S80);

            SS2 = _mm256_add_epi32(S01, S11);
            SS2 = _mm256_add_epi32(SS2, S21);
            SS2 = _mm256_add_epi32(SS2, S31);
            SS2 = _mm256_add_epi32(SS2, S41);
            SS2 = _mm256_add_epi32(SS2, S51);
            SS2 = _mm256_add_epi32(SS2, S61);
            SS2 = _mm256_add_epi32(SS2, S71);
            SS2 = _mm256_add_epi32(SS2, S81);

            SS1 = _mm256_add_epi32(SS1, mAddOffset);
            SS1 = _mm256_srai_epi32(SS1, 6);

            SS2 = _mm256_add_epi32(SS2, mAddOffset);
            SS2 = _mm256_srai_epi32(SS2, 6);

            S = _mm256_packus_epi32(SS1, SS2);
            S = _mm256_min_epu16(S, max_val);

            _mm256_store_si256((__m256i *)(dst + j), S);

        }

        src += i_src;
        dst += i_dst;
    }
}

#endif