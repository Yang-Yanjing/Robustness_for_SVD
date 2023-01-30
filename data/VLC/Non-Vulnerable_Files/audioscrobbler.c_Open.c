 *****************************************************************************/
static int Open(vlc_object_t *p_this)
{
    intf_thread_t   *p_intf     = (intf_thread_t*) p_this;
    intf_sys_t      *p_sys      = calloc(1, sizeof(intf_sys_t));
    if (!p_sys)
        return VLC_ENOMEM;
    p_intf->p_sys = p_sys;
    vlc_mutex_init(&p_sys->lock);
    vlc_cond_init(&p_sys->wait);
    if (vlc_clone(&p_sys->thread, Run, p_intf, VLC_THREAD_PRIORITY_LOW))
    {
        vlc_cond_destroy(&p_sys->wait);
        vlc_mutex_destroy(&p_sys->lock);
        free(p_sys);
        return VLC_ENOMEM;
    }
    var_AddCallback(pl_Get(p_intf), "activity", ItemChange, p_intf);
    return VLC_SUCCESS;
}
