}
static int exec_GenImage( filter_t *p_filter,
                          const commandparams_t *p_params,
                          commandparams_t *p_results )
{
    VLC_UNUSED(p_params);
    filter_sys_t *p_sys = (filter_sys_t*) p_filter->p_sys;
    overlay_t *p_ovl = OverlayCreate();
    if( p_ovl == NULL )
        return VLC_ENOMEM;
    ssize_t i_idx = ListAdd( &p_sys->overlays, p_ovl );
    if( i_idx < 0 )
        return i_idx;
    p_results->i_id = i_idx;
    return VLC_SUCCESS;
}
