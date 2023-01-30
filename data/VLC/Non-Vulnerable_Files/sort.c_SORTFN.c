}
SORTFN( SORT_URI, first, second )
{
    int i_ret;
    char *psz_first = input_item_GetURI( first->p_input );
    char *psz_second = input_item_GetURI( second->p_input );
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
