static int hvcc_parse_sps(GetBitContext *gb,
                          HEVCDecoderConfigurationRecord *hvcc)
{
    unsigned int i, sps_max_sub_layers_minus1, log2_max_pic_order_cnt_lsb_minus4;
    unsigned int num_short_term_ref_pic_sets, num_delta_pocs[MAX_SHORT_TERM_RPS_COUNT];
    skip_bits(gb, 4); 
    sps_max_sub_layers_minus1 = get_bits (gb, 3);
    







    hvcc->numTemporalLayers = FFMAX(hvcc->numTemporalLayers,
                                    sps_max_sub_layers_minus1 + 1);
    hvcc->temporalIdNested = get_bits1(gb);
    hvcc_parse_ptl(gb, hvcc, sps_max_sub_layers_minus1);
    get_ue_golomb_long(gb); 
    hvcc->chromaFormat = get_ue_golomb_long(gb);
    if (hvcc->chromaFormat == 3)
        skip_bits1(gb); 
    get_ue_golomb_long(gb); 
    get_ue_golomb_long(gb); 
    if (get_bits1(gb)) {        
        get_ue_golomb_long(gb); 
        get_ue_golomb_long(gb); 
        get_ue_golomb_long(gb); 
        get_ue_golomb_long(gb); 
    }
    hvcc->bitDepthLumaMinus8          = get_ue_golomb_long(gb);
    hvcc->bitDepthChromaMinus8        = get_ue_golomb_long(gb);
    log2_max_pic_order_cnt_lsb_minus4 = get_ue_golomb_long(gb);
    
    i = get_bits1(gb) ? 0 : sps_max_sub_layers_minus1;
    for (; i <= sps_max_sub_layers_minus1; i++)
        skip_sub_layer_ordering_info(gb);
    get_ue_golomb_long(gb); 
    get_ue_golomb_long(gb); 
    get_ue_golomb_long(gb); 
    get_ue_golomb_long(gb); 
    get_ue_golomb_long(gb); 
    get_ue_golomb_long(gb); 
    if (get_bits1(gb) && 
        get_bits1(gb))   
        skip_scaling_list_data(gb);
    skip_bits1(gb); 
    skip_bits1(gb); 
    if (get_bits1(gb)) {           
        skip_bits         (gb, 4); 
        skip_bits         (gb, 4); 
        get_ue_golomb_long(gb);    
        get_ue_golomb_long(gb);    
        skip_bits1        (gb);    
    }
    num_short_term_ref_pic_sets = get_ue_golomb_long(gb);
    if (num_short_term_ref_pic_sets > MAX_SHORT_TERM_RPS_COUNT)
        return AVERROR_INVALIDDATA;
    for (i = 0; i < num_short_term_ref_pic_sets; i++) {
        int ret = parse_rps(gb, i, num_short_term_ref_pic_sets, num_delta_pocs);
        if (ret < 0)
            return ret;
    }
    if (get_bits1(gb)) {                               
        unsigned num_long_term_ref_pics_sps = get_ue_golomb_long(gb);
        if (num_long_term_ref_pics_sps > 31U)
            return AVERROR_INVALIDDATA;
        for (i = 0; i < num_long_term_ref_pics_sps; i++) { 
            int len = FFMIN(log2_max_pic_order_cnt_lsb_minus4 + 4, 16);
            skip_bits (gb, len); 
            skip_bits1(gb);      
        }
    }
    skip_bits1(gb); 
    skip_bits1(gb); 
    if (get_bits1(gb)) 
        hvcc_parse_vui(gb, hvcc, sps_max_sub_layers_minus1);
    
    return 0;
}
