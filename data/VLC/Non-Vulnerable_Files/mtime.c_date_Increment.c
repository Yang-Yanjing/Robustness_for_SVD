 */
mtime_t date_Increment( date_t *p_date, uint32_t i_nb_samples )
{
    mtime_t i_dividend = i_nb_samples * CLOCK_FREQ * p_date->i_divider_den;
    lldiv_t d = lldiv( i_dividend, p_date->i_divider_num );
    p_date->date += d.quot;
    p_date->i_remainder += (int)d.rem;
    if( p_date->i_remainder >= p_date->i_divider_num )
    {
        /* This is Bresenham algorithm. */
        assert( p_date->i_remainder < 2*p_date->i_divider_num);
        p_date->date += 1;
        p_date->i_remainder -= p_date->i_divider_num;
    }
    return p_date->date;
}
