 */
static int Open(vlc_object_t *object)
{
    vout_display_t     *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
    /* Allocate instance and initialize some members */
    vd->sys = sys = calloc(1, sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    /* Does the framebuffer uses hw acceleration? */
    sys->is_hw_accel = var_InheritBool(vd, "fb-hw-accel");
    /* Set tty and fb devices */
    sys->tty = 0; /* 0 == /dev/tty0 == current console */
    sys->is_tty = var_InheritBool(vd, "fb-tty");
#if !defined(_WIN32) &&  defined(HAVE_ISATTY)
    /* Check that stdin is a TTY */
    if (sys->is_tty && !isatty(0)) {
        msg_Warn(vd, "standard input is not a TTY");
        free(sys);
        return VLC_EGENERIC;
    }
    msg_Warn(vd, "disabling TTY handling, use with caution because "
                 "there is no way to return to the TTY");
#endif
    const int mode = var_InheritInteger(vd, "fb-mode");
    bool force_resolution = true;
    switch (mode) {
    case 0: /* QCIF */
        sys->width  = 176;
        sys->height = 144;
        break;
    case 1: /* CIF */
        sys->width  = 352;
        sys->height = 288;
        break;
    case 2: /* NTSC */
        sys->width  = 640;
        sys->height = 480;
        break;
    case 3: /* PAL */
        sys->width  = 704;
        sys->height = 576;
        break;
    case 4:
    default:
        force_resolution = false;
        break;
    }
    char *chroma = var_InheritString(vd, "fb-chroma");
    if (chroma) {
        sys->chroma = vlc_fourcc_GetCodecFromString(VIDEO_ES, chroma);
        if (sys->chroma)
            msg_Dbg(vd, "forcing chroma '%s'", chroma);
        else
            msg_Warn(vd, "chroma %s invalid, using default", chroma);
        free(chroma);
    } else
        sys->chroma = 0;
    /* tty handling */
    if (sys->is_tty && TtyInit(vd)) {
        free(sys);
        return VLC_EGENERIC;
    }
    /* */
    sys->video_ptr = MAP_FAILED;
    sys->picture = NULL;
    sys->pool = NULL;
    if (OpenDisplay(vd, force_resolution)) {
        Close(VLC_OBJECT(vd));
        return VLC_EGENERIC;
    }
    vout_display_DeleteWindow(vd, NULL);
    /* */
    video_format_t fmt;
    video_format_ApplyRotation(&fmt, &vd->fmt);
    if (sys->chroma) {
        fmt.i_chroma = sys->chroma;
    } else {
        /* Assume RGB */
        msg_Dbg(vd, "%d bppd", sys->var_info.bits_per_pixel);
        switch (sys->var_info.bits_per_pixel) {
        case 8: /* FIXME: set the palette */
            fmt.i_chroma = VLC_CODEC_RGB8;
            break;
        case 15:
            fmt.i_chroma = VLC_CODEC_RGB15;
            break;
        case 16:
            fmt.i_chroma = VLC_CODEC_RGB16;
            break;
        case 24:
            fmt.i_chroma = VLC_CODEC_RGB24;
            break;
        case 32:
            fmt.i_chroma = VLC_CODEC_RGB32;
            break;
        default:
            msg_Err(vd, "unknown screendepth %i", sys->var_info.bits_per_pixel);
            Close(VLC_OBJECT(vd));
            return VLC_EGENERIC;
        }
        if (sys->var_info.bits_per_pixel != 8) {
            fmt.i_rmask = ((1 << sys->var_info.red.length) - 1)
                                 << sys->var_info.red.offset;
            fmt.i_gmask = ((1 << sys->var_info.green.length) - 1)
                                 << sys->var_info.green.offset;
            fmt.i_bmask = ((1 << sys->var_info.blue.length) - 1)
                                 << sys->var_info.blue.offset;
        }
    }
    fmt.i_visible_width  = sys->width;
    fmt.i_visible_height = sys->height;
    /* */
    vout_display_info_t info = vd->info;
    info.has_hide_mouse = true;
    /* */
    vd->fmt     = fmt;
    vd->info    = info;
    vd->pool    = Pool;
    vd->prepare = NULL;
    vd->display = Display;
    vd->control = Control;
    vd->manage  = NULL;
    /* */
    vout_display_SendEventFullscreen(vd, true);
    vout_display_SendEventDisplaySize(vd, fmt.i_visible_width, fmt.i_visible_height, true);
    return VLC_SUCCESS;
}
