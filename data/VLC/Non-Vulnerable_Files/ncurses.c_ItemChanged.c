}
static int ItemChanged(vlc_object_t *p_this, const char *variable,
                            vlc_value_t oval, vlc_value_t nval, void *param)
{
    VLC_UNUSED(p_this); VLC_UNUSED(variable);
    VLC_UNUSED(oval); VLC_UNUSED(nval);
    intf_sys_t *sys = ((intf_thread_t *)param)->p_sys;
    vlc_mutex_lock(&sys->pl_lock);
    sys->need_update = true;
    vlc_mutex_unlock(&sys->pl_lock);
    return VLC_SUCCESS;
}
