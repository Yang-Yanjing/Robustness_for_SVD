static void skip_timing_info(GetBitContext *gb)
{
    skip_bits_long(gb, 32); 
    skip_bits_long(gb, 32); 
    if (get_bits1(gb))          
        get_ue_golomb_long(gb); 
}
