}
void filter_chain_VideoFlush( filter_chain_t *p_chain )
{
    for( chained_filter_t *f = p_chain->first; f != NULL; f = f->next )
    {
        filter_t *p_filter = &f->filter;
        FilterDeletePictures( p_filter, f->pending );
        f->pending = NULL;
        filter_FlushPictures( p_filter );
    }
}
