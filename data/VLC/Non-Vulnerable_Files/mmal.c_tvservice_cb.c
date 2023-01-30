}
static void tvservice_cb(void *callback_data, uint32_t reason, uint32_t param1, uint32_t param2)
{
    VLC_UNUSED(reason);
    VLC_UNUSED(param1);
    VLC_UNUSED(param2);
    vout_display_t *vd = (vout_display_t *)callback_data;
    vout_display_sys_t *sys = vd->sys;
    vlc_mutex_lock(&sys->manage_mutex);
    sys->need_configure_display = true;
    vlc_mutex_unlock(&sys->manage_mutex);
}
