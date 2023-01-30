static int parse_rps(GetBitContext *gb, unsigned int rps_idx,
                     unsigned int num_rps,
                     unsigned int num_delta_pocs[MAX_SHORT_TERM_RPS_COUNT])
{
    unsigned int i;
    if (rps_idx && get_bits1(gb)) { 
        
        if (rps_idx >= num_rps)
            return AVERROR_INVALIDDATA;
        skip_bits1        (gb); 
        get_ue_golomb_long(gb); 
        num_delta_pocs[rps_idx] = 0;
        


















        for (i = 0; i <= num_delta_pocs[rps_idx - 1]; i++) {
            uint8_t use_delta_flag = 0;
            uint8_t used_by_curr_pic_flag = get_bits1(gb);
            if (!used_by_curr_pic_flag)
                use_delta_flag = get_bits1(gb);
            if (used_by_curr_pic_flag || use_delta_flag)
                num_delta_pocs[rps_idx]++;
        }
    } else {
        unsigned int num_negative_pics = get_ue_golomb_long(gb);
        unsigned int num_positive_pics = get_ue_golomb_long(gb);
        if ((num_positive_pics + (uint64_t)num_negative_pics) * 2 > get_bits_left(gb))
            return AVERROR_INVALIDDATA;
        num_delta_pocs[rps_idx] = num_negative_pics + num_positive_pics;
        for (i = 0; i < num_negative_pics; i++) {
            get_ue_golomb_long(gb); 
            skip_bits1        (gb); 
        }
        for (i = 0; i < num_positive_pics; i++) {
            get_ue_golomb_long(gb); 
            skip_bits1        (gb); 
        }
    }
    return 0;
}
