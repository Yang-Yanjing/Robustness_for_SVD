 */
static int UpdateVideoBufferFunctions( filter_chain_t *p_chain )
{
    /**
     * Last filter uses the filter chain's parent buffer allocation
     * functions. All the other filters use internal functions.
     * This makes it possible to have format changes between each
     * filter without having to worry about the parent's picture
     * heap format.
     */
    /* FIXME: we should only update the last and penultimate filters */
    chained_filter_t *f;
    for( f = p_chain->first; f != p_chain->last; f = f->next )
    {
        if( !IsInternalVideoAllocator( f ) )
        {
            AllocatorClean( &p_chain->allocator, f );
            AllocatorInit( &internal_video_allocator, f );
        }
    }
    if( f != NULL )
    {
        if( IsInternalVideoAllocator( f ) )
        {
            AllocatorClean( &internal_video_allocator, f );
            if( AllocatorInit( &p_chain->allocator, f ) )
                return VLC_EGENERIC;
        }
    }
    return VLC_SUCCESS;
}
