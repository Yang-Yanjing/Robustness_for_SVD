/* */
static int DirectXOpenDDraw(vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    HRESULT hr;
    /* */
    HRESULT (WINAPI *OurDirectDrawCreate)(GUID *,LPDIRECTDRAW *,IUnknown *);
    OurDirectDrawCreate =
        (void *)GetProcAddress(sys->hddraw_dll, "DirectDrawCreate");
    if (!OurDirectDrawCreate) {
        msg_Err(vd, "DirectXInitDDraw failed GetProcAddress");
        return VLC_EGENERIC;
    }
    /* */
    HRESULT (WINAPI *OurDirectDrawEnumerateEx)(LPDDENUMCALLBACKEXA, LPVOID, DWORD);
    OurDirectDrawEnumerateEx =
      (void *)GetProcAddress(sys->hddraw_dll, DIRECTDRAWENUMERATEEX_NAME);
    if (OurDirectDrawEnumerateEx) {
        char *device = var_GetString(vd, "directx-device");
        if (device) {
            msg_Dbg(vd, "directx-device: %s", device);
            free(device);
        }
        sys->hmonitor = MonitorFromWindow(sys->hwnd, MONITOR_DEFAULTTONEAREST);
        /* Enumerate displays */
        OurDirectDrawEnumerateEx(DirectXOpenDDrawCallback,
                                 vd, DDENUM_ATTACHEDSECONDARYDEVICES);
    }
    /* Initialize DirectDraw now */
    LPDIRECTDRAW ddobject;
    hr = OurDirectDrawCreate(sys->display_driver, &ddobject, NULL);
    if (hr != DD_OK) {
        msg_Err(vd, "DirectXInitDDraw cannot initialize DDraw");
        return VLC_EGENERIC;
    }
    /* Get the IDirectDraw2 interface */
    void *ptr;
    hr = IDirectDraw_QueryInterface(ddobject, &IID_IDirectDraw2,
                                    &ptr);
    /* Release the unused interface */
    IDirectDraw_Release(ddobject);
    if (hr != DD_OK) {
        msg_Err(vd, "cannot get IDirectDraw2 interface");
        sys->ddobject = NULL;
        return VLC_EGENERIC;
    }
    sys->ddobject = ptr;
    /* Set DirectDraw Cooperative level, ie what control we want over Windows
     * display */
    hr = IDirectDraw2_SetCooperativeLevel(sys->ddobject, NULL, DDSCL_NORMAL);
    if (hr != DD_OK) {
        msg_Err(vd, "cannot set direct draw cooperative level");
        return VLC_EGENERIC;
    }
    /* Get the size of the current display device */
    if (sys->hmonitor) {
        MONITORINFO monitor_info;
        monitor_info.cbSize = sizeof(MONITORINFO);
        GetMonitorInfoA(vd->sys->hmonitor, &monitor_info);
        sys->rect_display = monitor_info.rcMonitor;
    } else {
        sys->rect_display.left   = 0;
        sys->rect_display.top    = 0;
        sys->rect_display.right  = GetSystemMetrics(SM_CXSCREEN);
        sys->rect_display.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    msg_Dbg(vd, "screen dimensions (%lix%li,%lix%li)",
            sys->rect_display.left,
            sys->rect_display.top,
            sys->rect_display.right,
            sys->rect_display.bottom);
    /* Probe the capabilities of the hardware */
    DirectXGetDDrawCaps(vd);
    return VLC_SUCCESS;
}
