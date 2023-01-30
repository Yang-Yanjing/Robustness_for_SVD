}
static int exec_GetPosition( filter_t *p_filter,
                             const commandparams_t *p_params,
                             commandparams_t *p_results )
{
    filter_sys_t *p_sys = (filter_sys_t*) p_filter->p_sys;
    overlay_t *p_ovl = ListGet( &p_sys->overlays, p_params->i_id );
    if( p_ovl == NULL )
        return VLC_EGENERIC;
    p_results->i_x = p_ovl->i_x;
    p_results->i_y = p_ovl->i_y;
    return VLC_SUCCESS;
}
