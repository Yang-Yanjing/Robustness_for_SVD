 */
static inline int meta_sort( const playlist_item_t *first,
                             const playlist_item_t *second,
                             vlc_meta_type_t meta, bool b_integer )
{
    int i_ret;
    char *psz_first = input_item_GetMeta( first->p_input, meta );
    char *psz_second = input_item_GetMeta( second->p_input, meta );
    /* Nodes go first */
    if( first->i_children == -1 && second->i_children >= 0 )
        i_ret = 1;
    else if( first->i_children >= 0 && second->i_children == -1 )
       i_ret = -1;
    /* Both are nodes, sort by name */
    else if( first->i_children >= 0 && second->i_children >= 0 )
        i_ret = meta_strcasecmp_title( first, second );
    /* Both are items */
    else if( !psz_first && psz_second )
        i_ret = 1;
    else if( psz_first && !psz_second )
        i_ret = -1;
    /* No meta, sort by name */
    else if( !psz_first && !psz_second )
        i_ret = meta_strcasecmp_title( first, second );
    else
    {
        if( b_integer )
            i_ret = atoi( psz_first ) - atoi( psz_second );
        else
            i_ret = strcasecmp( psz_first, psz_second );
    }
    free( psz_first );
    free( psz_second );
    return i_ret;
}
