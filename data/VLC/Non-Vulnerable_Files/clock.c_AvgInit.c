 *****************************************************************************/
static void AvgInit( average_t *p_avg, int i_divider )
{
    p_avg->i_divider = i_divider;
    AvgReset( p_avg );
}
