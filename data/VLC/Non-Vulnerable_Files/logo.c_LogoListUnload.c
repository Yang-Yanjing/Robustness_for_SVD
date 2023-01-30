 */
static void LogoListUnload( logo_list_t *p_list )
{
    for( unsigned i = 0; i < p_list->i_count; i++ )
    {
        logo_t *p_logo = &p_list->p_logo[i];
        if( p_logo->p_pic )
            picture_Release( p_logo->p_pic );
    }
    free( p_list->p_logo );
}
