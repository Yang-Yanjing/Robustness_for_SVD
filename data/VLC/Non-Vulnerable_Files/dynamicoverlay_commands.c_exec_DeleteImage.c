}
static int exec_DeleteImage( filter_t *p_filter,
                             const commandparams_t *p_params,
                             commandparams_t *p_results )
{
    VLC_UNUSED(p_results);
    filter_sys_t *p_sys = (filter_sys_t*) p_filter->p_sys;
    p_sys->b_updated = true;
    return ListRemove( &p_sys->overlays, p_params->i_id );
}
