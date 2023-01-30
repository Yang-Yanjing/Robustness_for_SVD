static void nal_unit_parse_header(GetBitContext *gb, uint8_t *nal_type)
{
    skip_bits1(gb); 
    *nal_type = get_bits(gb, 6);
    



    skip_bits(gb, 9);
}
