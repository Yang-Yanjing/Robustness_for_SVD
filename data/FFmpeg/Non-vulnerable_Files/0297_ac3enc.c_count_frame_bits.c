static void count_frame_bits(AC3EncodeContext *s)
{
    AC3EncOptions *opt = &s->options;
    int blk, ch;
    int frame_bits = 0;
    
    if (s->eac3) {
        if (opt->eac3_mixing_metadata) {
            if (s->channel_mode > AC3_CHMODE_STEREO)
                frame_bits += 2;
            if (s->has_center)
                frame_bits += 6;
            if (s->has_surround)
                frame_bits += 6;
            frame_bits += s->lfe_on;
            frame_bits += 1 + 1 + 2;
            if (s->channel_mode < AC3_CHMODE_STEREO)
                frame_bits++;
            frame_bits++;
        }
        if (opt->eac3_info_metadata) {
            frame_bits += 3 + 1 + 1;
            if (s->channel_mode == AC3_CHMODE_STEREO)
                frame_bits += 2 + 2;
            if (s->channel_mode >= AC3_CHMODE_2F2R)
                frame_bits += 2;
            frame_bits++;
            if (opt->audio_production_info)
                frame_bits += 5 + 2 + 1;
            frame_bits++;
        }
        
        if (s->channel_mode > AC3_CHMODE_MONO) {
            frame_bits++;
            for (blk = 1; blk < s->num_blocks; blk++) {
                AC3Block *block = &s->blocks[blk];
                frame_bits++;
                if (block->new_cpl_strategy)
                    frame_bits++;
            }
        }
        
        if (s->cpl_on) {
            if (s->use_frame_exp_strategy) {
                frame_bits += 5 * s->cpl_on;
            } else {
                for (blk = 0; blk < s->num_blocks; blk++)
                    frame_bits += 2 * s->blocks[blk].cpl_in_use;
            }
        }
    } else {
        if (opt->audio_production_info)
            frame_bits += 7;
        if (s->bitstream_id == 6) {
            if (opt->extended_bsi_1)
                frame_bits += 14;
            if (opt->extended_bsi_2)
                frame_bits += 14;
        }
    }
    
    for (blk = 0; blk < s->num_blocks; blk++) {
        AC3Block *block = &s->blocks[blk];
        
        if (!s->eac3)
            frame_bits++;
        if (block->new_cpl_strategy) {
            if (!s->eac3)
                frame_bits++;
            if (block->cpl_in_use) {
                if (s->eac3)
                    frame_bits++;
                if (!s->eac3 || s->channel_mode != AC3_CHMODE_STEREO)
                    frame_bits += s->fbw_channels;
                if (s->channel_mode == AC3_CHMODE_STEREO)
                    frame_bits++;
                frame_bits += 4 + 4;
                if (s->eac3)
                    frame_bits++;
                else
                    frame_bits += s->num_cpl_subbands - 1;
            }
        }
        
        if (block->cpl_in_use) {
            for (ch = 1; ch <= s->fbw_channels; ch++) {
                if (block->channel_in_cpl[ch]) {
                    if (!s->eac3 || block->new_cpl_coords[ch] != 2)
                        frame_bits++;
                    if (block->new_cpl_coords[ch]) {
                        frame_bits += 2;
                        frame_bits += (4 + 4) * s->num_cpl_bands;
                    }
                }
            }
        }
        
        if (s->channel_mode == AC3_CHMODE_STEREO) {
            if (!s->eac3 || blk > 0)
                frame_bits++;
            if (s->blocks[blk].new_rematrixing_strategy)
                frame_bits += block->num_rematrixing_bands;
        }
        
        for (ch = 1; ch <= s->fbw_channels; ch++) {
            if (s->exp_strategy[ch][blk] != EXP_REUSE) {
                if (!block->channel_in_cpl[ch])
                    frame_bits += 6;
                frame_bits += 2;
            }
        }
        
        if (!s->eac3 && block->cpl_in_use)
            frame_bits += 2;
        
        if (!s->eac3) {
            frame_bits++;
            if (block->new_snr_offsets)
                frame_bits += 6 + (s->channels + block->cpl_in_use) * (4 + 3);
        }
        
        if (block->cpl_in_use) {
            if (!s->eac3 || block->new_cpl_leak != 2)
                frame_bits++;
            if (block->new_cpl_leak)
                frame_bits += 3 + 3;
        }
    }
    s->frame_bits = s->frame_bits_fixed + frame_bits;
}
