static int hvcc_parse_pps(GetBitContext *gb,
                          HEVCDecoderConfigurationRecord *hvcc)
{
    uint8_t tiles_enabled_flag, entropy_coding_sync_enabled_flag;
    get_ue_golomb_long(gb); 
    get_ue_golomb_long(gb); 
    






    skip_bits(gb, 7);
    get_ue_golomb_long(gb); 
    get_ue_golomb_long(gb); 
    get_se_golomb_long(gb); 
    



    skip_bits(gb, 2);
    if (get_bits1(gb))          
        get_ue_golomb_long(gb); 
    get_se_golomb_long(gb); 
    get_se_golomb_long(gb); 
    





    skip_bits(gb, 4);
    tiles_enabled_flag               = get_bits1(gb);
    entropy_coding_sync_enabled_flag = get_bits1(gb);
    if (entropy_coding_sync_enabled_flag && tiles_enabled_flag)
        hvcc->parallelismType = 0; 
    else if (entropy_coding_sync_enabled_flag)
        hvcc->parallelismType = 3; 
    else if (tiles_enabled_flag)
        hvcc->parallelismType = 2; 
    else
        hvcc->parallelismType = 1; 
    
    return 0;
}
