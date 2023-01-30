 */
void date_Set( date_t *p_date, mtime_t i_new_date )
{
    p_date->date = i_new_date;
    p_date->i_remainder = 0;
}
