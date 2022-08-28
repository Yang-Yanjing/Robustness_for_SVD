static int decode_spectrum_and_dequant(AACContext *ac, INTFLOAT coef[1024],
                                       GetBitContext *gb, const INTFLOAT sf[120],
                                       int pulse_present, const Pulse *pulse,
                                       const IndividualChannelStream *ics,
                                       enum BandType band_type[120])
{
    int i, k, g, idx = 0;
    const int c = 1024 / ics->num_windows;
    const uint16_t *offsets = ics->swb_offset;
    INTFLOAT *coef_base = coef;
    for (g = 0; g < ics->num_windows; g++)
        memset(coef + g * 128 + offsets[ics->max_sfb], 0,
               sizeof(INTFLOAT) * (c - offsets[ics->max_sfb]));
    for (g = 0; g < ics->num_window_groups; g++) {
        unsigned g_len = ics->group_len[g];
        for (i = 0; i < ics->max_sfb; i++, idx++) {
            const unsigned cbt_m1 = band_type[idx] - 1;
            INTFLOAT *cfo = coef + offsets[i];
            int off_len = offsets[i + 1] - offsets[i];
            int group;
            if (cbt_m1 >= INTENSITY_BT2 - 1) {
                for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
                    memset(cfo, 0, off_len * sizeof(*cfo));
                }
            } else if (cbt_m1 == NOISE_BT - 1) {
                for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
#if !USE_FIXED
                    float scale;
#endif 
                    INTFLOAT band_energy;
                    for (k = 0; k < off_len; k++) {
                        ac->random_state  = lcg_random(ac->random_state);
#if USE_FIXED
                        cfo[k] = ac->random_state >> 3;
#else
                        cfo[k] = ac->random_state;
#endif 
                    }
#if USE_FIXED
                    band_energy = ac->fdsp->scalarproduct_fixed(cfo, cfo, off_len);
                    band_energy = fixed_sqrt(band_energy, 31);
                    noise_scale(cfo, sf[idx], band_energy, off_len);
#else
                    band_energy = ac->fdsp->scalarproduct_float(cfo, cfo, off_len);
                    scale = sf[idx] / sqrtf(band_energy);
                    ac->fdsp->vector_fmul_scalar(cfo, cfo, scale, off_len);
#endif 
                }
            } else {
#if !USE_FIXED
                const float *vq = ff_aac_codebook_vector_vals[cbt_m1];
#endif 
                const uint16_t *cb_vector_idx = ff_aac_codebook_vector_idx[cbt_m1];
                VLC_TYPE (*vlc_tab)[2] = vlc_spectral[cbt_m1].table;
                OPEN_READER(re, gb);
                switch (cbt_m1 >> 1) {
                case 0:
                    for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
                        INTFLOAT *cf = cfo;
                        int len = off_len;
                        do {
                            int code;
                            unsigned cb_idx;
                            UPDATE_CACHE(re, gb);
                            GET_VLC(code, re, gb, vlc_tab, 8, 2);
                            cb_idx = cb_vector_idx[code];
#if USE_FIXED
                            cf = DEC_SQUAD(cf, cb_idx);
#else
                            cf = VMUL4(cf, vq, cb_idx, sf + idx);
#endif 
                        } while (len -= 4);
                    }
                    break;
                case 1:
                    for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
                        INTFLOAT *cf = cfo;
                        int len = off_len;
                        do {
                            int code;
                            unsigned nnz;
                            unsigned cb_idx;
                            uint32_t bits;
                            UPDATE_CACHE(re, gb);
                            GET_VLC(code, re, gb, vlc_tab, 8, 2);
                            cb_idx = cb_vector_idx[code];
                            nnz = cb_idx >> 8 & 15;
                            bits = nnz ? GET_CACHE(re, gb) : 0;
                            LAST_SKIP_BITS(re, gb, nnz);
#if USE_FIXED
                            cf = DEC_UQUAD(cf, cb_idx, bits);
#else
                            cf = VMUL4S(cf, vq, cb_idx, bits, sf + idx);
#endif 
                        } while (len -= 4);
                    }
                    break;
                case 2:
                    for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
                        INTFLOAT *cf = cfo;
                        int len = off_len;
                        do {
                            int code;
                            unsigned cb_idx;
                            UPDATE_CACHE(re, gb);
                            GET_VLC(code, re, gb, vlc_tab, 8, 2);
                            cb_idx = cb_vector_idx[code];
#if USE_FIXED
                            cf = DEC_SPAIR(cf, cb_idx);
#else
                            cf = VMUL2(cf, vq, cb_idx, sf + idx);
#endif 
                        } while (len -= 2);
                    }
                    break;
                case 3:
                case 4:
                    for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
                        INTFLOAT *cf = cfo;
                        int len = off_len;
                        do {
                            int code;
                            unsigned nnz;
                            unsigned cb_idx;
                            unsigned sign;
                            UPDATE_CACHE(re, gb);
                            GET_VLC(code, re, gb, vlc_tab, 8, 2);
                            cb_idx = cb_vector_idx[code];
                            nnz = cb_idx >> 8 & 15;
                            sign = nnz ? SHOW_UBITS(re, gb, nnz) << (cb_idx >> 12) : 0;
                            LAST_SKIP_BITS(re, gb, nnz);
#if USE_FIXED
                            cf = DEC_UPAIR(cf, cb_idx, sign);
#else
                            cf = VMUL2S(cf, vq, cb_idx, sign, sf + idx);
#endif 
                        } while (len -= 2);
                    }
                    break;
                default:
                    for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
#if USE_FIXED
                        int *icf = cfo;
                        int v;
#else
                        float *cf = cfo;
                        uint32_t *icf = (uint32_t *) cf;
#endif 
                        int len = off_len;
                        do {
                            int code;
                            unsigned nzt, nnz;
                            unsigned cb_idx;
                            uint32_t bits;
                            int j;
                            UPDATE_CACHE(re, gb);
                            GET_VLC(code, re, gb, vlc_tab, 8, 2);
                            if (!code) {
                                *icf++ = 0;
                                *icf++ = 0;
                                continue;
                            }
                            cb_idx = cb_vector_idx[code];
                            nnz = cb_idx >> 12;
                            nzt = cb_idx >> 8;
                            bits = SHOW_UBITS(re, gb, nnz) << (32-nnz);
                            LAST_SKIP_BITS(re, gb, nnz);
                            for (j = 0; j < 2; j++) {
                                if (nzt & 1<<j) {
                                    uint32_t b;
                                    int n;
                                    

                                    UPDATE_CACHE(re, gb);
                                    b = GET_CACHE(re, gb);
                                    b = 31 - av_log2(~b);
                                    if (b > 8) {
                                        av_log(ac->avctx, AV_LOG_ERROR, "error in spectral data, ESC overflow\n");
                                        return AVERROR_INVALIDDATA;
                                    }
                                    SKIP_BITS(re, gb, b + 1);
                                    b += 4;
                                    n = (1 << b) + SHOW_UBITS(re, gb, b);
                                    LAST_SKIP_BITS(re, gb, b);
#if USE_FIXED
                                    v = n;
                                    if (bits & 1U<<31)
                                        v = -v;
                                    *icf++ = v;
#else
                                    *icf++ = cbrt_tab[n] | (bits & 1U<<31);
#endif 
                                    bits <<= 1;
                                } else {
#if USE_FIXED
                                    v = cb_idx & 15;
                                    if (bits & 1U<<31)
                                        v = -v;
                                    *icf++ = v;
#else
                                    unsigned v = ((const uint32_t*)vq)[cb_idx & 15];
                                    *icf++ = (bits & 1U<<31) | v;
#endif 
                                    bits <<= !!v;
                                }
                                cb_idx >>= 4;
                            }
                        } while (len -= 2);
#if !USE_FIXED
                        ac->fdsp->vector_fmul_scalar(cfo, cfo, sf[idx], off_len);
#endif 
                    }
                }
                CLOSE_READER(re, gb);
            }
        }
        coef += g_len << 7;
    }
    if (pulse_present) {
        idx = 0;
        for (i = 0; i < pulse->num_pulse; i++) {
            INTFLOAT co = coef_base[ pulse->pos[i] ];
            while (offsets[idx + 1] <= pulse->pos[i])
                idx++;
            if (band_type[idx] != NOISE_BT && sf[idx]) {
                INTFLOAT ico = -pulse->amp[i];
#if USE_FIXED
                if (co) {
                    ico = co + (co > 0 ? -ico : ico);
                }
                coef_base[ pulse->pos[i] ] = ico;
#else
                if (co) {
                    co /= sf[idx];
                    ico = co / sqrtf(sqrtf(fabsf(co))) + (co > 0 ? -ico : ico);
                }
                coef_base[ pulse->pos[i] ] = cbrtf(fabsf(ico)) * ico * sf[idx];
#endif 
            }
        }
    }
#if USE_FIXED
    coef = coef_base;
    idx = 0;
    for (g = 0; g < ics->num_window_groups; g++) {
        unsigned g_len = ics->group_len[g];
        for (i = 0; i < ics->max_sfb; i++, idx++) {
            const unsigned cbt_m1 = band_type[idx] - 1;
            int *cfo = coef + offsets[i];
            int off_len = offsets[i + 1] - offsets[i];
            int group;
            if (cbt_m1 < NOISE_BT - 1) {
                for (group = 0; group < (int)g_len; group++, cfo+=128) {
                    ac->vector_pow43(cfo, off_len);
                    ac->subband_scale(cfo, cfo, sf[idx], 34, off_len);
                }
            }
        }
        coef += g_len << 7;
    }
#endif 
    return 0;
}
