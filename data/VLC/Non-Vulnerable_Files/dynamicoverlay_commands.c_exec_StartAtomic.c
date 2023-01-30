}
static int exec_StartAtomic( filter_t *p_filter,
                             const commandparams_t *p_params,
                             commandparams_t *p_results )
{
    filter_sys_t *p_sys = (filter_sys_t*) p_filter->p_sys;
    VLC_UNUSED(p_params);
    VLC_UNUSED(p_results);
    p_sys->b_atomic = true;
    return VLC_SUCCESS;
}
