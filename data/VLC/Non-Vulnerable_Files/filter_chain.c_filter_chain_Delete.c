 */
void filter_chain_Delete( filter_chain_t *p_chain )
{
    filter_chain_Reset( p_chain, NULL, NULL );
    es_format_Clean( &p_chain->fmt_in );
    es_format_Clean( &p_chain->fmt_out );
    free( p_chain );
}
