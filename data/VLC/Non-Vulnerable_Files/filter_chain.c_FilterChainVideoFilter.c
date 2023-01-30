}
static picture_t *FilterChainVideoFilter( chained_filter_t *f, picture_t *p_pic )
{
    for( ; f != NULL; f = f->next )
    {
        filter_t *p_filter = &f->filter;
        p_pic = p_filter->pf_video_filter( p_filter, p_pic );
        if( !p_pic )
            break;
        if( f->pending )
        {
            msg_Warn( p_filter, "dropping pictures" );
            FilterDeletePictures( p_filter, f->pending );
        }
        f->pending = p_pic->p_next;
        p_pic->p_next = NULL;
    }
    return p_pic;
}
