/* */
static int AllocatorInit( const filter_chain_allocator_t *p_alloc,
                          chained_filter_t *p_filter )
{
    if( p_alloc->pf_init )
        return p_alloc->pf_init( &p_filter->filter, p_alloc->p_data );
    return VLC_SUCCESS;
}
