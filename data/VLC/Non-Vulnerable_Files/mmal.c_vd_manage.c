}
static void vd_manage(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    unsigned width, height;
    vlc_mutex_lock(&sys->manage_mutex);
    if (sys->need_configure_display) {
        close_dmx(vd);
        sys->dmx_handle = vc_dispmanx_display_open(0);
        if (query_resolution(vd, &width, &height) >= 0) {
            sys->display_width = width;
            sys->display_height = height;
            vout_display_SendEventDisplaySize(vd, width, height, vd->cfg->is_fullscreen);
        }
        sys->need_configure_display = false;
    }
    vlc_mutex_unlock(&sys->manage_mutex);
}
