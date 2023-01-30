static void
gdigrab_region_wnd_destroy(AVFormatContext *s1, struct gdigrab *gdigrab)
{
    if (gdigrab->region_hwnd)
        DestroyWindow(gdigrab->region_hwnd);
    gdigrab->region_hwnd = NULL;
}
