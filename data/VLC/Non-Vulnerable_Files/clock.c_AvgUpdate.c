}
static void AvgUpdate( average_t *p_avg, mtime_t i_value )
{
    const int i_f0 = __MIN( p_avg->i_divider - 1, p_avg->i_count );
    const int i_f1 = p_avg->i_divider - i_f0;
    const mtime_t i_tmp = i_f0 * p_avg->i_value + i_f1 * i_value + p_avg->i_residue;
    p_avg->i_value   = i_tmp / p_avg->i_divider;
    p_avg->i_residue = i_tmp % p_avg->i_divider;
    p_avg->i_count++;
}
