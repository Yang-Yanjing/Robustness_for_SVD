/* */
static BOOL WINAPI DirectXOpenDDrawCallback(GUID *guid, LPSTR desc,
                                            LPSTR drivername, VOID *context,
                                            HMONITOR hmon)
{
    vout_display_t *vd = context;
    vout_display_sys_t *sys = vd->sys;
    /* This callback function is called by DirectDraw once for each
     * available DirectDraw device.
     *
     * Returning TRUE keeps enumerating.
     */
    if (!hmon)
        return TRUE;
    char *psz_drivername = drivername;
    char *psz_desc = desc;
    msg_Dbg(vd, "DirectXEnumCallback: %s, %s", psz_desc, psz_drivername);
    char *device = var_GetString(vd, "directx-device");
    /* Check for forced device */
    if (device && *device && !strcmp(psz_drivername, device)) {
        MONITORINFO monitor_info;
        monitor_info.cbSize = sizeof(MONITORINFO);
        if (GetMonitorInfoA(hmon, &monitor_info)) {
            RECT rect;
            /* Move window to the right screen */
            GetWindowRect(sys->hwnd, &rect);
            if (!IntersectRect(&rect, &rect, &monitor_info.rcWork)) {
                rect.left = monitor_info.rcWork.left;
                rect.top = monitor_info.rcWork.top;
                msg_Dbg(vd, "DirectXEnumCallback: setting window "
                            "position to %ld,%ld", rect.left, rect.top);
                SetWindowPos(sys->hwnd, NULL,
                             rect.left, rect.top, 0, 0,
                             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
        sys->hmonitor = hmon;
    }
    free(device);
    if (hmon == sys->hmonitor) {
        msg_Dbg(vd, "selecting %s, %s", psz_desc, psz_drivername);
        free(sys->display_driver);
        sys->display_driver = malloc(sizeof(*guid));
        if (sys->display_driver)
            *sys->display_driver = *guid;
    }
    return TRUE;
}
