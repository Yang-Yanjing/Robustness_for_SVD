static int x11grab_read_header(AVFormatContext *s1)
{
    X11GrabContext *x11grab = s1->priv_data;
    Display *dpy;
    AVStream *st = NULL;
    XImage *image;
    int x_off = 0, y_off = 0, ret = 0, screen, use_shm = 0;
    char *dpyname, *offset;
    Colormap color_map;
    XColor color[256];
    int i;
    dpyname = av_strdup(s1->filename);
    if (!dpyname)
        goto out;
    offset = strchr(dpyname, '+');
    if (offset) {
        sscanf(offset, "%d,%d", &x_off, &y_off);
        if (strstr(offset, "nomouse")) {
            av_log(s1, AV_LOG_WARNING,
                   "'nomouse' specification in argument is deprecated: "
                   "use 'draw_mouse' option with value 0 instead\n");
            x11grab->draw_mouse = 0;
        }
        *offset = 0;
    }
    av_log(s1, AV_LOG_INFO,
           "device: %s -> display: %s x: %d y: %d width: %d height: %d\n",
           s1->filename, dpyname, x_off, y_off, x11grab->width, x11grab->height);
    dpy = XOpenDisplay(dpyname);
    av_freep(&dpyname);
    if (!dpy) {
        av_log(s1, AV_LOG_ERROR, "Could not open X display.\n");
        ret = AVERROR(EIO);
        goto out;
    }
    st = avformat_new_stream(s1, NULL);
    if (!st) {
        ret = AVERROR(ENOMEM);
        goto out;
    }
    avpriv_set_pts_info(st, 64, 1, 1000000); 
    screen = DefaultScreen(dpy);
    if (x11grab->follow_mouse) {
        int screen_w, screen_h;
        Window w;
        screen_w = DisplayWidth(dpy, screen);
        screen_h = DisplayHeight(dpy, screen);
        XQueryPointer(dpy, RootWindow(dpy, screen), &w, &w, &x_off, &y_off,
                      &ret, &ret, &ret);
        x_off -= x11grab->width / 2;
        y_off -= x11grab->height / 2;
        x_off = av_clip(x_off, 0, screen_w - x11grab->width);
        y_off = av_clip(y_off, 0, screen_h - x11grab->height);
        av_log(s1, AV_LOG_INFO,
               "followmouse is enabled, resetting grabbing region to x: %d y: %d\n",
               x_off, y_off);
    }
    if (x11grab->use_shm) {
        use_shm = XShmQueryExtension(dpy);
        av_log(s1, AV_LOG_INFO,
               "shared memory extension %sfound\n", use_shm ? "" : "not ");
    }
    if (use_shm && setup_shm(s1, dpy, &image) < 0) {
        av_log(s1, AV_LOG_WARNING, "Falling back to XGetImage\n");
        use_shm = 0;
    }
    if (!use_shm) {
        image = XGetImage(dpy, RootWindow(dpy, screen),
                          x_off, y_off,
                          x11grab->width, x11grab->height,
                          AllPlanes, ZPixmap);
    }
    if (x11grab->draw_mouse && setup_mouse(dpy, screen) < 0) {
        av_log(s1, AV_LOG_WARNING,
               "XFixes not available, cannot draw the mouse cursor\n");
        x11grab->draw_mouse = 0;
    }
    x11grab->frame_size = x11grab->width * x11grab->height * image->bits_per_pixel / 8;
    x11grab->dpy        = dpy;
    x11grab->time_base  = av_inv_q(x11grab->framerate);
    x11grab->time_frame = av_gettime() / av_q2d(x11grab->time_base);
    x11grab->x_off      = x_off;
    x11grab->y_off      = y_off;
    x11grab->image      = image;
    x11grab->use_shm    = use_shm;
    ret = pixfmt_from_image(s1, image, &st->codec->pix_fmt);
    if (ret < 0)
        goto out;
    if (st->codec->pix_fmt == AV_PIX_FMT_PAL8) {
        color_map = DefaultColormap(dpy, screen);
        for (i = 0; i < 256; ++i)
            color[i].pixel = i;
        XQueryColors(dpy, color_map, color, 256);
        for (i = 0; i < 256; ++i)
            x11grab->palette[i] = (color[i].red   & 0xFF00) << 8 |
                                  (color[i].green & 0xFF00)      |
                                  (color[i].blue  & 0xFF00) >> 8;
        x11grab->palette_changed = 1;
    }
    st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    st->codec->codec_id   = AV_CODEC_ID_RAWVIDEO;
    st->codec->width      = x11grab->width;
    st->codec->height     = x11grab->height;
    st->codec->time_base  = x11grab->time_base;
    st->codec->bit_rate   = x11grab->frame_size * 1 / av_q2d(x11grab->time_base) * 8;
out:
    av_free(dpyname);
    return ret;
}
