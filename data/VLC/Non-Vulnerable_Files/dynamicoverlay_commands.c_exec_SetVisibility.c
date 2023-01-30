}
static int exec_SetVisibility( filter_t *p_filter,
                               const commandparams_t *p_params,
                               commandparams_t *p_results )
{
    VLC_UNUSED(p_results);
    filter_sys_t *p_sys = (filter_sys_t*) p_filter->p_sys;
    overlay_t *p_ovl = ListGet( &p_sys->overlays, p_params->i_id );
    if( p_ovl == NULL )
        return VLC_EGENERIC;
    p_ovl->b_active = p_params->b_visible;// ? false : true;
    p_sys->b_updated = true;
    return VLC_SUCCESS;
}
