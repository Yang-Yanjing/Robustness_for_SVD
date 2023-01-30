}
int filter_chain_DeleteFilter( filter_chain_t *p_chain, filter_t *p_filter )
{
    const int i_ret = filter_chain_DeleteFilterInternal( p_chain, p_filter );
    if( i_ret < 0 )
        return i_ret;
    /* FIXME That one seems bad if a error is returned */
    return UpdateBufferFunctions( p_chain );
}
