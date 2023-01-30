 */
mtime_t date_Decrement( date_t *p_date, uint32_t i_nb_samples )
{
    mtime_t i_dividend = (mtime_t)i_nb_samples * CLOCK_FREQ * p_date->i_divider_den;
    p_date->date -= i_dividend / p_date->i_divider_num;
    unsigned i_rem_adjust = i_dividend % p_date->i_divider_num;
    if( p_date->i_remainder < i_rem_adjust )
    {
        /* This is Bresenham algorithm. */
        assert( p_date->i_remainder < p_date->i_divider_num);
        p_date->date -= 1;
        p_date->i_remainder += p_date->i_divider_num;
    }
    p_date->i_remainder -= i_rem_adjust;
    return p_date->date;
}
