 */
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
#if !defined(__APPLE__) && !defined(_WIN32)
# ifndef X_DISPLAY_MISSING
    if (!vlc_xlib_init(object))
        return VLC_EGENERIC;
# endif
#endif
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    SMALL_RECT rect;
    COORD coord;
    HANDLE hstdout;
    if (!AllocConsole()) {
        msg_Err(vd, "cannot create console");
        return VLC_EGENERIC;
    }
    hstdout =
        CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if (!hstdout || hstdout == INVALID_HANDLE_VALUE) {
        msg_Err(vd, "cannot create screen buffer");
        FreeConsole();
        return VLC_EGENERIC;
    }
    if (!SetConsoleActiveScreenBuffer(hstdout)) {
        msg_Err(vd, "cannot set active screen buffer");
        FreeConsole();
        return VLC_EGENERIC;
    }
    coord = GetLargestConsoleWindowSize(hstdout);
    msg_Dbg(vd, "SetConsoleWindowInfo: %ix%i", coord.X, coord.Y);
    /* Force size for now */
    coord.X = 100;
    coord.Y = 40;
    if (!SetConsoleScreenBufferSize(hstdout, coord))
        msg_Warn(vd, "SetConsoleScreenBufferSize %i %i",
                  coord.X, coord.Y);
    /* Get the current screen buffer size and window position. */
    if (GetConsoleScreenBufferInfo(hstdout, &csbiInfo)) {
        rect.Top = 0; rect.Left = 0;
        rect.Right = csbiInfo.dwMaximumWindowSize.X - 1;
        rect.Bottom = csbiInfo.dwMaximumWindowSize.Y - 1;
        if (!SetConsoleWindowInfo(hstdout, TRUE, &rect))
            msg_Dbg(vd, "SetConsoleWindowInfo failed: %ix%i",
                     rect.Right, rect.Bottom);
    }
#endif
    /* Allocate structure */
    vd->sys = sys = calloc(1, sizeof(*sys));
    if (!sys)
        goto error;
    sys->cv = cucul_create_canvas(0, 0);
    if (!sys->cv) {
        msg_Err(vd, "cannot initialize libcucul");
        goto error;
    }
    const char *driver = NULL;
#ifdef __APPLE__
    // Make sure we don't try to open a window.
    driver = "ncurses";
#endif
    sys->dp = caca_create_display_with_driver(sys->cv, driver);
    if (!sys->dp) {
        msg_Err(vd, "cannot initialize libcaca");
        goto error;
    }
    vout_display_DeleteWindow(vd, NULL);
    if (vd->cfg->display.title)
        caca_set_display_title(sys->dp,
                               vd->cfg->display.title);
    else
        caca_set_display_title(sys->dp,
                               VOUT_TITLE "(Colour AsCii Art)");
    /* Fix format */
    video_format_t fmt = vd->fmt;
    if (fmt.i_chroma != VLC_CODEC_RGB32) {
        fmt.i_chroma = VLC_CODEC_RGB32;
        fmt.i_rmask = 0x00ff0000;
        fmt.i_gmask = 0x0000ff00;
        fmt.i_bmask = 0x000000ff;
    }
    /* TODO */
    vout_display_info_t info = vd->info;
    /* Setup vout_display now that everything is fine */
    vd->fmt = fmt;
    vd->info = info;
    vd->pool    = Pool;
    vd->prepare = Prepare;
    vd->display = PictureDisplay;
    vd->control = Control;
    vd->manage  = Manage;
    /* Fix initial state */
    vout_display_SendEventFullscreen(vd, false);
    Refresh(vd);
    return VLC_SUCCESS;
error:
    if (sys) {
        if (sys->pool)
            picture_pool_Delete(sys->pool);
        if (sys->dither)
            cucul_free_dither(sys->dither);
        if (sys->dp)
            caca_free_display(sys->dp);
        if (sys->cv)
            cucul_free_canvas(sys->cv);
        free(sys);
    }
#if defined(_WIN32)
    FreeConsole();
#endif
    return VLC_EGENERIC;
}
