}
int filter_chain_MouseFilter( filter_chain_t *p_chain, vlc_mouse_t *p_dst, const vlc_mouse_t *p_src )
{
    vlc_mouse_t current = *p_src;
    for( chained_filter_t *f = p_chain->last; f != NULL; f = f->prev )
    {
        filter_t *p_filter = &f->filter;
        vlc_mouse_t *p_mouse = f->mouse;
        if( p_filter->pf_video_mouse && p_mouse )
        {
            vlc_mouse_t old = *p_mouse;
            vlc_mouse_t filtered;
            *p_mouse = current;
            if( p_filter->pf_video_mouse( p_filter, &filtered, &old, &current ) )
                return VLC_EGENERIC;
            current = filtered;
        }
    }
    *p_dst = current;
    return VLC_SUCCESS;
}
