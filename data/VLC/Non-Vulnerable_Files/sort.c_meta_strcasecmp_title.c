 */
static inline int meta_strcasecmp_title( const playlist_item_t *first,
                              const playlist_item_t *second )
{
    int i_ret;
    char *psz_first = input_item_GetTitleFbName( first->p_input );
    char *psz_second = input_item_GetTitleFbName( second->p_input );
    if( psz_first && psz_second )
        i_ret = strcasecmp( psz_first, psz_second );
    else if( !psz_first && psz_second )
        i_ret = 1;
    else if( psz_first && !psz_second )
        i_ret = -1;
    else
        i_ret = 0;
    free( psz_first );
    free( psz_second );
    return i_ret;
}
