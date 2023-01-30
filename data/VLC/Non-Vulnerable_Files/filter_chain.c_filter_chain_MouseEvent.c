}
int filter_chain_MouseEvent( filter_chain_t *p_chain,
                             const vlc_mouse_t *p_mouse,
                             const video_format_t *p_fmt )
{
    for( chained_filter_t *f = p_chain->first; f != NULL; f = f->next )
    {
        filter_t *p_filter = &f->filter;
        if( p_filter->pf_sub_mouse )
        {
            vlc_mouse_t old = *f->mouse;
            *f->mouse = *p_mouse;
            if( p_filter->pf_sub_mouse( p_filter, &old, p_mouse, p_fmt ) )
                return VLC_EGENERIC;
        }
    }
    return VLC_SUCCESS;
}
