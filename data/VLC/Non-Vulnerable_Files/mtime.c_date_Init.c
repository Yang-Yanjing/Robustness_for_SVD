 */
void date_Init( date_t *p_date, uint32_t i_divider_n, uint32_t i_divider_d )
{
    p_date->date = 0;
    p_date->i_divider_num = i_divider_n;
    p_date->i_divider_den = i_divider_d;
    p_date->i_remainder = 0;
}
