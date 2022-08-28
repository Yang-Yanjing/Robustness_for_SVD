static void hvcc_parse_ptl(GetBitContext *gb,
                           HEVCDecoderConfigurationRecord *hvcc,
                           unsigned int max_sub_layers_minus1)
{
    unsigned int i;
    HVCCProfileTierLevel general_ptl;
    uint8_t sub_layer_profile_present_flag[MAX_SUB_LAYERS];
    uint8_t sub_layer_level_present_flag[MAX_SUB_LAYERS];
    general_ptl.profile_space               = get_bits(gb, 2);
    general_ptl.tier_flag                   = get_bits1(gb);
    general_ptl.profile_idc                 = get_bits(gb, 5);
    general_ptl.profile_compatibility_flags = get_bits_long(gb, 32);
    general_ptl.constraint_indicator_flags  = get_bits64(gb, 48);
    general_ptl.level_idc                   = get_bits(gb, 8);
    hvcc_update_ptl(hvcc, &general_ptl);
    for (i = 0; i < max_sub_layers_minus1; i++) {
        sub_layer_profile_present_flag[i] = get_bits1(gb);
        sub_layer_level_present_flag[i]   = get_bits1(gb);
    }
    if (max_sub_layers_minus1 > 0)
        for (i = max_sub_layers_minus1; i < 8; i++)
            skip_bits(gb, 2); 
    for (i = 0; i < max_sub_layers_minus1; i++) {
        if (sub_layer_profile_present_flag[i]) {
            










            skip_bits_long(gb, 32);
            skip_bits_long(gb, 32);
            skip_bits     (gb, 24);
        }
        if (sub_layer_level_present_flag[i])
            skip_bits(gb, 8);
    }
}
