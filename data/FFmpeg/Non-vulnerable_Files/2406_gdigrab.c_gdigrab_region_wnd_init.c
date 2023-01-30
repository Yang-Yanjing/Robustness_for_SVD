static int
gdigrab_region_wnd_init(AVFormatContext *s1, struct gdigrab *gdigrab)
{
    HWND hwnd;
    RECT rect = gdigrab->clip_rect;
    HRGN region = NULL;
    HRGN region_interior = NULL;
    DWORD style = WS_POPUP | WS_VISIBLE;
    DWORD ex = WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT;
    rect.left -= REGION_WND_BORDER; rect.top -= REGION_WND_BORDER;
    rect.right += REGION_WND_BORDER; rect.bottom += REGION_WND_BORDER;
    AdjustWindowRectEx(&rect, style, FALSE, ex);
    
    
    hwnd = CreateWindowEx(ex, WC_DIALOG, NULL, style, rect.left, rect.top,
            rect.right - rect.left, rect.bottom - rect.top,
            NULL, NULL, NULL, NULL);
    if (!hwnd) {
        WIN32_API_ERROR("Could not create region display window");
        goto error;
    }
    
    GetClientRect(hwnd, &rect);
    region = CreateRectRgn(0, 0,
            rect.right - rect.left, rect.bottom - rect.top);
    region_interior = CreateRectRgn(REGION_WND_BORDER, REGION_WND_BORDER,
            rect.right - rect.left - REGION_WND_BORDER,
            rect.bottom - rect.top - REGION_WND_BORDER);
    CombineRgn(region, region, region_interior, RGN_DIFF);
    if (!SetWindowRgn(hwnd, region, FALSE)) {
        WIN32_API_ERROR("Could not set window region");
        goto error;
    }
    
    region = NULL;
    DeleteObject(region_interior);
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR) gdigrab_region_wnd_proc);
    ShowWindow(hwnd, SW_SHOW);
    gdigrab->region_hwnd = hwnd;
    return 0;
error:
    if (region)
        DeleteObject(region);
    if (region_interior)
        DeleteObject(region_interior);
    if (hwnd)
        DestroyWindow(hwnd);
    return 1;
}
