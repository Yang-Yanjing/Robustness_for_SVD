 */
filter_chain_t *filter_chain_New( vlc_object_t *p_this,
                                  const char *psz_capability,
                                  bool b_allow_fmt_out_change,
                                  int  (*pf_buffer_allocation_init)( filter_t *, void * ),
                                  void (*pf_buffer_allocation_clean)( filter_t * ),
                                  void *p_buffer_allocation_data )
{
    assert( p_this );
    assert( psz_capability );
    size_t size = sizeof(filter_chain_t) + strlen(psz_capability);
    filter_chain_t *p_chain = malloc( size );
    if( !p_chain )
        return NULL;
    p_chain->p_this = p_this;
    p_chain->last = p_chain->first = NULL;
    p_chain->length = 0;
    strcpy( p_chain->psz_capability, psz_capability );
    es_format_Init( &p_chain->fmt_in, UNKNOWN_ES, 0 );
    es_format_Init( &p_chain->fmt_out, UNKNOWN_ES, 0 );
    p_chain->b_allow_fmt_out_change = b_allow_fmt_out_change;
    p_chain->allocator.pf_init = pf_buffer_allocation_init;
    p_chain->allocator.pf_clean = pf_buffer_allocation_clean;
    p_chain->allocator.p_data = p_buffer_allocation_data;
    return p_chain;
}
