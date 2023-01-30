}
static void VoutDisplayFitWindow(vout_display_t *vd, bool default_size)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    vout_display_cfg_t cfg = osys->cfg;
    if (!cfg.is_display_filled)
        return;
    cfg.display.width = 0;
    if (default_size) {
        cfg.display.height = 0;
    } else {
        cfg.display.height = osys->height_saved;
        cfg.zoom.num = 1;
        cfg.zoom.den = 1;
    }
    unsigned display_width;
    unsigned display_height;
    vout_display_GetDefaultDisplaySize(&display_width, &display_height,
                                       &vd->source, &cfg);
    vlc_mutex_lock(&osys->lock);
    osys->ch_display_size       = true;
    osys->display_width         = display_width;
    osys->display_height        = display_height;
    osys->display_is_fullscreen = osys->cfg.is_fullscreen;
    osys->display_is_forced     = true;
    vlc_mutex_unlock(&osys->lock);
}
