}
static int filter_chain_DeleteFilterInternal( filter_chain_t *p_chain,
                                              filter_t *p_filter )
{
    chained_filter_t *p_chained = chained( p_filter );
    /* Remove it from the chain */
    if( p_chained->prev != NULL )
        p_chained->prev->next = p_chained->next;
    else
    {
        assert( p_chained == p_chain->first );
        p_chain->first = p_chained->next;
    }
    if( p_chained->next != NULL )
        p_chained->next->prev = p_chained->prev;
    else
    {
        assert( p_chained == p_chain->last );
        p_chain->last = p_chained->prev;
    }
    p_chain->length--;
    msg_Dbg( p_chain->p_this, "Filter %p removed from chain", p_filter );
    FilterDeletePictures( &p_chained->filter, p_chained->pending );
    /* Destroy the filter object */
    if( IsInternalVideoAllocator( p_chained ) )
        AllocatorClean( &internal_video_allocator, p_chained );
    else
        AllocatorClean( &p_chain->allocator, p_chained );
    if( p_filter->p_module )
        module_unneed( p_filter, p_filter->p_module );
    free( p_chained->mouse );
    vlc_object_release( p_filter );
    /* FIXME: check fmt_in/fmt_out consitency */
    return VLC_SUCCESS;
}
