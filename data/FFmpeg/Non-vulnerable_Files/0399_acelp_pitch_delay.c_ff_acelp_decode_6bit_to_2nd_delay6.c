int ff_acelp_decode_6bit_to_2nd_delay6(
        int ac_index,
        int pitch_delay_min)
{
    return 6 * pitch_delay_min + ac_index - 3;
}
