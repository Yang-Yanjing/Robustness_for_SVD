static int decode_scalefactors(AACContext *ac, INTFLOAT sf[120], GetBitContext *gb,
                               unsigned int global_gain,
                               IndividualChannelStream *ics,
                               enum BandType band_type[120],
                               int band_type_run_end[120])
{
    int g, i, idx = 0;
    int offset[3] = { global_gain, global_gain - NOISE_OFFSET, 0 };
    int clipped_offset;
    int noise_flag = 1;
    for (g = 0; g < ics->num_window_groups; g++) {
        for (i = 0; i < ics->max_sfb;) {
            int run_end = band_type_run_end[idx];
            if (band_type[idx] == ZERO_BT) {
                for (; i < run_end; i++, idx++)
                    sf[idx] = FIXR(0.);
            } else if ((band_type[idx] == INTENSITY_BT) ||
                       (band_type[idx] == INTENSITY_BT2)) {
                for (; i < run_end; i++, idx++) {
                    offset[2] += get_vlc2(gb, vlc_scalefactors.table, 7, 3) - SCALE_DIFF_ZERO;
                    clipped_offset = av_clip(offset[2], -155, 100);
                    if (offset[2] != clipped_offset) {
                        avpriv_request_sample(ac->avctx,
                                              "If you heard an audible artifact, there may be a bug in the decoder. "
                                              "Clipped intensity stereo position (%d -> %d)",
                                              offset[2], clipped_offset);
                    }
#if USE_FIXED
                    sf[idx] = 100 - clipped_offset;
#else
                    sf[idx] = ff_aac_pow2sf_tab[-clipped_offset + POW_SF2_ZERO];
#endif 
                }
            } else if (band_type[idx] == NOISE_BT) {
                for (; i < run_end; i++, idx++) {
                    if (noise_flag-- > 0)
                        offset[1] += get_bits(gb, NOISE_PRE_BITS) - NOISE_PRE;
                    else
                        offset[1] += get_vlc2(gb, vlc_scalefactors.table, 7, 3) - SCALE_DIFF_ZERO;
                    clipped_offset = av_clip(offset[1], -100, 155);
                    if (offset[1] != clipped_offset) {
                        avpriv_request_sample(ac->avctx,
                                              "If you heard an audible artifact, there may be a bug in the decoder. "
                                              "Clipped noise gain (%d -> %d)",
                                              offset[1], clipped_offset);
                    }
#if USE_FIXED
                    sf[idx] = -(100 + clipped_offset);
#else
                    sf[idx] = -ff_aac_pow2sf_tab[clipped_offset + POW_SF2_ZERO];
#endif 
                }
            } else {
                for (; i < run_end; i++, idx++) {
                    offset[0] += get_vlc2(gb, vlc_scalefactors.table, 7, 3) - SCALE_DIFF_ZERO;
                    if (offset[0] > 255U) {
                        av_log(ac->avctx, AV_LOG_ERROR,
                               "Scalefactor (%d) out of range.\n", offset[0]);
                        return AVERROR_INVALIDDATA;
                    }
#if USE_FIXED
                    sf[idx] = -offset[0];
#else
                    sf[idx] = -ff_aac_pow2sf_tab[offset[0] - 100 + POW_SF2_ZERO];
#endif 
                }
            }
        }
    }
    return 0;
}
