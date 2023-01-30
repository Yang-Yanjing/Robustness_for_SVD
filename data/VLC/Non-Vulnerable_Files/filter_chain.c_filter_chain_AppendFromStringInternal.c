}
static int filter_chain_AppendFromStringInternal( filter_chain_t *p_chain,
                                                  const char *psz_string )
{
    config_chain_t *p_cfg = NULL;
    char *psz_name = NULL;
    char* psz_new_string;
    if( !psz_string || !*psz_string )
        return 0;
    psz_new_string = config_ChainCreate( &psz_name, &p_cfg, psz_string );
    filter_t *p_filter = filter_chain_AppendFilterInternal( p_chain, psz_name,
                                                            p_cfg, NULL, NULL );
    if( !p_filter )
    {
        msg_Err( p_chain->p_this, "Failed while trying to append '%s' "
                 "to filter chain", psz_name );
        free( psz_name );
        free( p_cfg );
        free( psz_new_string );
        return -1;
    }
    free( psz_name );
    const int i_ret = filter_chain_AppendFromStringInternal( p_chain, psz_new_string );
    free( psz_new_string );
    if( i_ret < 0 )
    {
        filter_chain_DeleteFilterInternal( p_chain, p_filter );
        return i_ret;
    }
    return 1 + i_ret;
}
