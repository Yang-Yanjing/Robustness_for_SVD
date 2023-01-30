int ff_acelp_decode_8bit_to_1st_delay3(int ac_index)
{
    ac_index += 58;
    if(ac_index > 254)
        ac_index = 3 * ac_index - 510;
    return ac_index;
}
