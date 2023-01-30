/* */
static void WallpaperChange(vout_display_t *vd, bool use_wallpaper)
{
    vout_display_sys_t *sys = vd->sys;
    if (!sys->use_wallpaper == !use_wallpaper)
        return;
    HWND hwnd = FindWindow(_T("Progman"), NULL);
    if (hwnd)
        hwnd = FindWindowEx(hwnd, NULL, _T("SHELLDLL_DefView"), NULL);
    if (hwnd)
        hwnd = FindWindowEx(hwnd, NULL, _T("SysListView32"), NULL);
    if (!hwnd) {
        msg_Warn(vd, "couldn't find \"SysListView32\" window, "
                     "wallpaper mode not supported");
        return;
    }
    msg_Dbg(vd, "wallpaper mode %s", use_wallpaper ? "enabled" : "disabled");
    sys->use_wallpaper = use_wallpaper;
    if (sys->use_wallpaper) {
        sys->color_bkg    = ListView_GetBkColor(hwnd);
        sys->color_bkgtxt = ListView_GetTextBkColor(hwnd);
        ListView_SetBkColor(hwnd,     sys->i_rgb_colorkey);
        ListView_SetTextBkColor(hwnd, sys->i_rgb_colorkey);
    } else {
        ListView_SetBkColor(hwnd,     sys->color_bkg);
        ListView_SetTextBkColor(hwnd, sys->color_bkgtxt);
    }
    /* Update desktop */
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
    if (sys->use_overlay)
      DirectXUpdateOverlay(vd, NULL);
}
