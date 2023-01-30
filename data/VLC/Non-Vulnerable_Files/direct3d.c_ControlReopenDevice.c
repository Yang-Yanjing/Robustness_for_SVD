}
static int ControlReopenDevice(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    if (!sys->use_desktop) {
        /* Save non-desktop state */
        sys->desktop_save.is_fullscreen = vd->cfg->is_fullscreen;
        sys->desktop_save.is_on_top     = sys->is_on_top;
        WINDOWPLACEMENT wp = { .length = sizeof(wp), };
        GetWindowPlacement(sys->hparent ? sys->hparent : sys->hwnd, &wp);
        sys->desktop_save.win = wp.rcNormalPosition;
    }
    /* */
    Direct3DClose(vd);
    EventThreadStop(sys->event);
    /* */
    vlc_mutex_lock(&sys->lock);
    sys->use_desktop = sys->desktop_requested;
    sys->ch_desktop = false;
    vlc_mutex_unlock(&sys->lock);
    /* */
    event_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.use_desktop = sys->use_desktop;
    if (!sys->use_desktop) {
        cfg.win.type   = VOUT_WINDOW_TYPE_HWND;
        cfg.win.x      = sys->desktop_save.win.left;
        cfg.win.y      = sys->desktop_save.win.top;
        cfg.win.width  = sys->desktop_save.win.right  - sys->desktop_save.win.left;
        cfg.win.height = sys->desktop_save.win.bottom - sys->desktop_save.win.top;
    }
    event_hwnd_t hwnd;
    if (EventThreadStart(sys->event, &hwnd, &cfg)) {
        msg_Err(vd, "Failed to restart event thread");
        return VLC_EGENERIC;
    }
    sys->parent_window = hwnd.parent_window;
    sys->hparent       = hwnd.hparent;
    sys->hwnd          = hwnd.hwnd;
    sys->hvideownd     = hwnd.hvideownd;
    sys->hfswnd        = hwnd.hfswnd;
    SetRectEmpty(&sys->rect_parent);
    /* */
    video_format_t fmt;
    if (Direct3DOpen(vd, &fmt)) {
        CommonClean(vd);
        msg_Err(vd, "Failed to reopen device");
        return VLC_EGENERIC;
    }
    vd->fmt = fmt;
    sys->is_first_display = true;
    if (sys->use_desktop) {
        /* Disable fullscreen/on_top while using desktop */
        if (sys->desktop_save.is_fullscreen)
            vout_display_SendEventFullscreen(vd, false);
        if (sys->desktop_save.is_on_top)
            vout_display_SendWindowState(vd, VOUT_WINDOW_STATE_NORMAL);
    } else {
        /* Restore fullscreen/on_top */
        if (sys->desktop_save.is_fullscreen)
            vout_display_SendEventFullscreen(vd, true);
        if (sys->desktop_save.is_on_top)
            vout_display_SendWindowState(vd, VOUT_WINDOW_STATE_ABOVE);
    }
    return VLC_SUCCESS;
}
