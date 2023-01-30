}
static void AvgRescale( average_t *p_avg, int i_divider )
{
    const mtime_t i_tmp = p_avg->i_value * p_avg->i_divider + p_avg->i_residue;
    p_avg->i_divider = i_divider;
    p_avg->i_value   = i_tmp / p_avg->i_divider;
    p_avg->i_residue = i_tmp % p_avg->i_divider;
}
