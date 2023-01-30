 */
static logo_t *LogoListNext( logo_list_t *p_list, mtime_t i_date )
{
    p_list->i_counter = ( p_list->i_counter + 1 ) % p_list->i_count;
    logo_t *p_logo = LogoListCurrent( p_list );
    p_list->i_next_pic = i_date + ( p_logo->i_delay != -1 ?
                          p_logo->i_delay : p_list->i_delay ) * 1000;
    return p_logo;
}
