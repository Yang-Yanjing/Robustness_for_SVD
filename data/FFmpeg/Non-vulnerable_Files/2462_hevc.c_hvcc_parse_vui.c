static void hvcc_parse_vui(GetBitContext *gb,
                           HEVCDecoderConfigurationRecord *hvcc,
                           unsigned int max_sub_layers_minus1)
{
    unsigned int min_spatial_segmentation_idc;
    if (get_bits1(gb))              
        if (get_bits(gb, 8) == 255) 
            skip_bits_long(gb, 32); 
    if (get_bits1(gb))  
        skip_bits1(gb); 
    if (get_bits1(gb)) {  
        skip_bits(gb, 4); 
        if (get_bits1(gb)) 
            




            skip_bits(gb, 24);
    }
    if (get_bits1(gb)) {        
        get_ue_golomb_long(gb); 
        get_ue_golomb_long(gb); 
    }
    




    skip_bits(gb, 3);
    if (get_bits1(gb)) {        
        get_ue_golomb_long(gb); 
        get_ue_golomb_long(gb); 
        get_ue_golomb_long(gb); 
        get_ue_golomb_long(gb); 
    }
    if (get_bits1(gb)) { 
        skip_timing_info(gb);
        if (get_bits1(gb)) 
            skip_hrd_parameters(gb, 1, max_sub_layers_minus1);
    }
    if (get_bits1(gb)) { 
        




        skip_bits(gb, 3);
        min_spatial_segmentation_idc = get_ue_golomb_long(gb);
        






        hvcc->min_spatial_segmentation_idc = FFMIN(hvcc->min_spatial_segmentation_idc,
                                                   min_spatial_segmentation_idc);
        get_ue_golomb_long(gb); 
        get_ue_golomb_long(gb); 
        get_ue_golomb_long(gb); 
        get_ue_golomb_long(gb); 
    }
}
