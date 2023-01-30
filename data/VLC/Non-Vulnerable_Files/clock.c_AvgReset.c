}
static void AvgReset( average_t *p_avg )
{
    p_avg->i_value = 0;
    p_avg->i_residue = 0;
    p_avg->i_count = 0;
}
