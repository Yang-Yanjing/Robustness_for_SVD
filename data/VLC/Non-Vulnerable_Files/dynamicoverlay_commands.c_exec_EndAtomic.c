}
static int exec_EndAtomic( filter_t *p_filter,
                           const commandparams_t *p_params,
                           commandparams_t *p_results )
{
    VLC_UNUSED(p_params);
    VLC_UNUSED(p_results);
    filter_sys_t *p_sys = (filter_sys_t*) p_filter->p_sys;
    QueueTransfer( &p_sys->pending, &p_sys->atomic );
    p_sys->b_atomic = false;
    return VLC_SUCCESS;
}
