}
static void AllocatorClean( const filter_chain_allocator_t *p_alloc,
                            chained_filter_t *p_filter )
{
    if( p_alloc->pf_clean )
        p_alloc->pf_clean( &p_filter->filter );
}
