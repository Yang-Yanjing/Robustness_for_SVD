static void
gdigrab_region_wnd_update(AVFormatContext *s1, struct gdigrab *gdigrab)
{
    HWND hwnd = gdigrab->region_hwnd;
    MSG msg;
    while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
        DispatchMessage(&msg);
    }
}
