static int
gdigrab_read_header(AVFormatContext *s1)
{
    struct gdigrab *gdigrab = s1->priv_data;
    HWND hwnd;
    HDC source_hdc = NULL;
    HDC dest_hdc   = NULL;
    BITMAPINFO bmi;
    HBITMAP hbmp   = NULL;
    void *buffer   = NULL;
    const char *filename = s1->filename;
    const char *name     = NULL;
    AVStream   *st       = NULL;
    int bpp;
    RECT virtual_rect;
    RECT clip_rect;
    BITMAP bmp;
    int ret;
    if (!strncmp(filename, "title=", 6)) {
        name = filename + 6;
        hwnd = FindWindow(NULL, name);
        if (!hwnd) {
            av_log(s1, AV_LOG_ERROR,
                   "Can't find window '%s', aborting.\n", name);
            ret = AVERROR(EIO);
            goto error;
        }
        if (gdigrab->show_region) {
            av_log(s1, AV_LOG_WARNING,
                    "Can't show region when grabbing a window.\n");
            gdigrab->show_region = 0;
        }
    } else if (!strcmp(filename, "desktop")) {
        hwnd = NULL;
    } else {
        av_log(s1, AV_LOG_ERROR,
               "Please use \"desktop\" or \"title=<windowname>\" to specify your target.\n");
        ret = AVERROR(EIO);
        goto error;
    }
    if (hwnd) {
        GetClientRect(hwnd, &virtual_rect);
    } else {
        virtual_rect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        virtual_rect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
        virtual_rect.right = virtual_rect.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        virtual_rect.bottom = virtual_rect.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }
    
    if (!gdigrab->width || !gdigrab->height) {
        clip_rect.left = virtual_rect.left;
        clip_rect.top = virtual_rect.top;
        clip_rect.right = virtual_rect.right;
        clip_rect.bottom = virtual_rect.bottom;
    } else {
        clip_rect.left = gdigrab->offset_x;
        clip_rect.top = gdigrab->offset_y;
        clip_rect.right = gdigrab->width + gdigrab->offset_x;
        clip_rect.bottom = gdigrab->height + gdigrab->offset_y;
    }
    if (clip_rect.left < virtual_rect.left ||
            clip_rect.top < virtual_rect.top ||
            clip_rect.right > virtual_rect.right ||
            clip_rect.bottom > virtual_rect.bottom) {
            av_log(s1, AV_LOG_ERROR,
                    "Capture area (%li,%li),(%li,%li) extends outside window area (%li,%li),(%li,%li)",
                    clip_rect.left, clip_rect.top,
                    clip_rect.right, clip_rect.bottom,
                    virtual_rect.left, virtual_rect.top,
                    virtual_rect.right, virtual_rect.bottom);
            ret = AVERROR(EIO);
            goto error;
    }
    

    source_hdc = GetDC(hwnd);
    if (!source_hdc) {
        WIN32_API_ERROR("Couldn't get window device context");
        ret = AVERROR(EIO);
        goto error;
    }
    bpp = GetDeviceCaps(source_hdc, BITSPIXEL);
    if (name) {
        av_log(s1, AV_LOG_INFO,
               "Found window %s, capturing %lix%lix%i at (%li,%li)\n",
               name,
               clip_rect.right - clip_rect.left,
               clip_rect.bottom - clip_rect.top,
               bpp, clip_rect.left, clip_rect.top);
    } else {
        av_log(s1, AV_LOG_INFO,
               "Capturing whole desktop as %lix%lix%i at (%li,%li)\n",
               clip_rect.right - clip_rect.left,
               clip_rect.bottom - clip_rect.top,
               bpp, clip_rect.left, clip_rect.top);
    }
    if (clip_rect.right - clip_rect.left <= 0 ||
            clip_rect.bottom - clip_rect.top <= 0 || bpp%8) {
        av_log(s1, AV_LOG_ERROR, "Invalid properties, aborting\n");
        ret = AVERROR(EIO);
        goto error;
    }
    dest_hdc = CreateCompatibleDC(source_hdc);
    if (!dest_hdc) {
        WIN32_API_ERROR("Screen DC CreateCompatibleDC");
        ret = AVERROR(EIO);
        goto error;
    }
    
    bmi.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth         = clip_rect.right - clip_rect.left;
    bmi.bmiHeader.biHeight        = -(clip_rect.bottom - clip_rect.top);
    bmi.bmiHeader.biPlanes        = 1;
    bmi.bmiHeader.biBitCount      = bpp;
    bmi.bmiHeader.biCompression   = BI_RGB;
    bmi.bmiHeader.biSizeImage     = 0;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    bmi.bmiHeader.biClrUsed       = 0;
    bmi.bmiHeader.biClrImportant  = 0;
    hbmp = CreateDIBSection(dest_hdc, &bmi, DIB_RGB_COLORS,
            &buffer, NULL, 0);
    if (!hbmp) {
        WIN32_API_ERROR("Creating DIB Section");
        ret = AVERROR(EIO);
        goto error;
    }
    if (!SelectObject(dest_hdc, hbmp)) {
        WIN32_API_ERROR("SelectObject");
        ret = AVERROR(EIO);
        goto error;
    }
    
    GetObject(hbmp, sizeof(BITMAP), &bmp);
    st = avformat_new_stream(s1, NULL);
    if (!st) {
        ret = AVERROR(ENOMEM);
        goto error;
    }
    avpriv_set_pts_info(st, 64, 1, 1000000); 
    gdigrab->frame_size  = bmp.bmWidthBytes * bmp.bmHeight * bmp.bmPlanes;
    gdigrab->header_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                           (bpp <= 8 ? (1 << bpp) : 0) * sizeof(RGBQUAD) ;
    gdigrab->time_base   = av_inv_q(gdigrab->framerate);
    gdigrab->time_frame  = av_gettime() / av_q2d(gdigrab->time_base);
    gdigrab->hwnd       = hwnd;
    gdigrab->source_hdc = source_hdc;
    gdigrab->dest_hdc   = dest_hdc;
    gdigrab->hbmp       = hbmp;
    gdigrab->bmi        = bmi;
    gdigrab->buffer     = buffer;
    gdigrab->clip_rect  = clip_rect;
    gdigrab->cursor_error_printed = 0;
    if (gdigrab->show_region) {
        if (gdigrab_region_wnd_init(s1, gdigrab)) {
            ret = AVERROR(EIO);
            goto error;
        }
    }
    st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    st->codec->codec_id   = AV_CODEC_ID_BMP;
    st->codec->time_base  = gdigrab->time_base;
    st->codec->bit_rate   = (gdigrab->header_size + gdigrab->frame_size) * 1/av_q2d(gdigrab->time_base) * 8;
    return 0;
error:
    if (source_hdc)
        ReleaseDC(hwnd, source_hdc);
    if (dest_hdc)
        DeleteDC(dest_hdc);
    if (hbmp)
        DeleteObject(hbmp);
    if (source_hdc)
        DeleteDC(source_hdc);
    return ret;
}
