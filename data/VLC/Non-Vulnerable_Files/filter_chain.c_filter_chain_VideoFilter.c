}
picture_t *filter_chain_VideoFilter( filter_chain_t *p_chain, picture_t *p_pic )
{
    if( p_pic )
    {
        p_pic = FilterChainVideoFilter( p_chain->first, p_pic );
        if( p_pic )
            return p_pic;
    }
    for( chained_filter_t *b = p_chain->last; b != NULL; b = b->prev )
    {
        p_pic = b->pending;
        if( !p_pic )
            continue;
        b->pending = p_pic->p_next;
        p_pic->p_next = NULL;
        p_pic = FilterChainVideoFilter( b->next, p_pic );
        if( p_pic )
            return p_pic;
    }
    return NULL;
}
