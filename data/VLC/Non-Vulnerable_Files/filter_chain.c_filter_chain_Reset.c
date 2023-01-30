 */
void filter_chain_Reset( filter_chain_t *p_chain, const es_format_t *p_fmt_in,
                         const es_format_t *p_fmt_out )
{
    while( p_chain->first != NULL )
        filter_chain_DeleteFilterInternal( p_chain, &p_chain->first->filter );
    if( p_fmt_in )
    {
        es_format_Clean( &p_chain->fmt_in );
        es_format_Copy( &p_chain->fmt_in, p_fmt_in );
    }
    if( p_fmt_out )
    {
        es_format_Clean( &p_chain->fmt_out );
        es_format_Copy( &p_chain->fmt_out, p_fmt_out );
    }
}
