}
static int Callback(vlc_object_t *object, char const *cmd,
                    vlc_value_t oldval, vlc_value_t newval, void *data)
{
    filter_t     *filter = (filter_t *)object;
    filter_sys_t *sys = filter->p_sys;
    VLC_UNUSED(oldval); VLC_UNUSED(data);
    vlc_mutex_lock(&sys->lock);
    if (!strcmp(cmd, CFG_PREFIX "strength"))
        sys->strength = newval.f_float;
    else
        sys->radius    = newval.i_int;
    vlc_mutex_unlock(&sys->lock);
    return VLC_SUCCESS;
}
