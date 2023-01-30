 */
void date_Change( date_t *p_date, uint32_t i_divider_n, uint32_t i_divider_d )
{
    /* change time scale of remainder */
    p_date->i_remainder = p_date->i_remainder * i_divider_n / p_date->i_divider_num;
    p_date->i_divider_num = i_divider_n;
    p_date->i_divider_den = i_divider_d;
}
