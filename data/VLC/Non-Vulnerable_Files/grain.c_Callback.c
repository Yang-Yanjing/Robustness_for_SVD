}
static int Callback(vlc_object_t *object, char const *cmd,
                    vlc_value_t oldval, vlc_value_t newval, void *data)
{
    filter_t     *filter = (filter_t *)object;
    filter_sys_t *sys = filter->p_sys;
    VLC_UNUSED(cmd); VLC_UNUSED(oldval); VLC_UNUSED(data);
    vlc_mutex_lock(&sys->cfg.lock);
    sys->cfg.variance = newval.f_float;
    vlc_mutex_unlock(&sys->cfg.lock);
    return VLC_SUCCESS;
}
