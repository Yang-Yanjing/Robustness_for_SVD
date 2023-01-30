static int Control (vout_display_t *, int, va_list);
static vout_window_t *MakeWindow (vout_display_t *vd)
{
    vout_window_cfg_t cfg = {
        .x = var_InheritInteger (vd, "video-x"),
        .y = var_InheritInteger (vd, "video-y"),
        .width = vd->cfg->display.width,
        .height = vd->cfg->display.height,
    };
    vout_window_t *wnd;
#if defined(_WIN32)
    cfg.type = VOUT_WINDOW_TYPE_HWND;
#elif defined(__ANDROID__)
    cfg.type = VOUT_WINDOW_TYPE_ANDROID_NATIVE;
#else
    cfg.type = VOUT_WINDOW_TYPE_XID;
#endif
    wnd = vout_display_NewWindow (vd, &cfg);
    if (wnd != NULL)
        return wnd;
    msg_Err (vd, "parent window not available");
    return NULL;
}
