int ff_acelp_decode_4bit_to_2nd_delay3(
        int ac_index,
        int pitch_delay_min)
{
    if(ac_index < 4)
        return 3 * (ac_index + pitch_delay_min);
    else if(ac_index < 12)
        return 3 * pitch_delay_min + ac_index + 6;
    else
        return 3 * (ac_index + pitch_delay_min) - 18;
}
