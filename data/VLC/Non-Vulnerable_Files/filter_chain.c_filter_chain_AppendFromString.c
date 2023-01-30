}
int filter_chain_AppendFromString( filter_chain_t *p_chain,
                                   const char *psz_string )
{
    const int i_ret = filter_chain_AppendFromStringInternal( p_chain, psz_string );
    if( i_ret < 0 )
        return i_ret;
    /* FIXME That one seems bad if a error is returned */
    return UpdateBufferFunctions( p_chain );
}
