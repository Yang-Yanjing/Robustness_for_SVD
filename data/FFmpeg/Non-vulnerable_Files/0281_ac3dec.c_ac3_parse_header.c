static int ac3_parse_header(AC3DecodeContext *s)
{
    GetBitContext *gbc = &s->gbc;
    int i;
    
    i = !s->channel_mode;
    do {
        s->dialog_normalization[(!s->channel_mode)-i] = -get_bits(gbc, 5);
        if (s->dialog_normalization[(!s->channel_mode)-i] == 0) {
            s->dialog_normalization[(!s->channel_mode)-i] = -31;
        }
        if (s->target_level != 0) {
            s->level_gain[(!s->channel_mode)-i] = powf(2.0f,
                (float)(s->target_level -
                s->dialog_normalization[(!s->channel_mode)-i])/6.0f);
        }
        if (s->compression_exists[(!s->channel_mode)-i] = get_bits1(gbc)) {
            s->heavy_dynamic_range[(!s->channel_mode)-i] =
                AC3_HEAVY_RANGE(get_bits(gbc, 8));
        }
        if (get_bits1(gbc))
            skip_bits(gbc, 8); 
        if (get_bits1(gbc))
            skip_bits(gbc, 7); 
    } while (i--);
    skip_bits(gbc, 2); 
    
    if (s->bitstream_id != 6) {
        if (get_bits1(gbc))
            skip_bits(gbc, 14); 
        if (get_bits1(gbc))
            skip_bits(gbc, 14); 
    } else {
        if (get_bits1(gbc)) {
            s->preferred_downmix       = get_bits(gbc, 2);
            s->center_mix_level_ltrt   = get_bits(gbc, 3);
            s->surround_mix_level_ltrt = av_clip(get_bits(gbc, 3), 3, 7);
            s->center_mix_level        = get_bits(gbc, 3);
            s->surround_mix_level      = av_clip(get_bits(gbc, 3), 3, 7);
        }
        if (get_bits1(gbc)) {
            s->dolby_surround_ex_mode = get_bits(gbc, 2);
            s->dolby_headphone_mode   = get_bits(gbc, 2);
            skip_bits(gbc, 10); 
        }
    }
    
    if (get_bits1(gbc)) {
        i = get_bits(gbc, 6);
        do {
            skip_bits(gbc, 8);
        } while (i--);
    }
    return 0;
}
