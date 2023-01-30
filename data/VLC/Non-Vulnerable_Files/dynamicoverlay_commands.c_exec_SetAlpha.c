}
static int exec_SetAlpha( filter_t *p_filter,
                          const commandparams_t *p_params,
                          commandparams_t *p_results )
{
    VLC_UNUSED(p_results);
    filter_sys_t *p_sys = (filter_sys_t*) p_filter->p_sys;
    overlay_t *p_ovl = ListGet( &p_sys->overlays, p_params->i_id );
    if( p_ovl == NULL )
        return VLC_EGENERIC;
    p_ovl->i_alpha = p_params->i_alpha;
    p_sys->b_updated = p_ovl->b_active;
    return VLC_SUCCESS;
}
