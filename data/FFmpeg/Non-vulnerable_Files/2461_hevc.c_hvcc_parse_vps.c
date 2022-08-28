static int hvcc_parse_vps(GetBitContext *gb,
                          HEVCDecoderConfigurationRecord *hvcc)
{
    unsigned int vps_max_sub_layers_minus1;
    




    skip_bits(gb, 12);
    vps_max_sub_layers_minus1 = get_bits(gb, 3);
    







    hvcc->numTemporalLayers = FFMAX(hvcc->numTemporalLayers,
                                    vps_max_sub_layers_minus1 + 1);
    



    skip_bits(gb, 17);
    hvcc_parse_ptl(gb, hvcc, vps_max_sub_layers_minus1);
    
    return 0;
}
