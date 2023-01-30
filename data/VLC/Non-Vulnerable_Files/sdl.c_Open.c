 */
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
#if !defined(_WIN32) && !defined(__OS2__)
    if (!vlc_xlib_init (object))
        return VLC_EGENERIC;
#endif
    /* XXX: check for conflicts with the SDL audio output */
    vlc_mutex_lock(&sdl_lock);
    /* Check if SDL video module has been initialized */
    if (SDL_WasInit(SDL_INIT_VIDEO) != 0) {
        vlc_mutex_unlock(&sdl_lock);
        return VLC_EGENERIC;
    }
    vd->sys = sys = calloc(1, sizeof(*sys));
    if (!sys) {
        vlc_mutex_unlock(&sdl_lock);
        return VLC_ENOMEM;
    }
    /* */
    int sdl_flags = SDL_INIT_VIDEO;
#ifndef _WIN32
    /* Win32 SDL implementation doesn't support SDL_INIT_EVENTTHREAD yet*/
    sdl_flags |= SDL_INIT_EVENTTHREAD;
#endif
    /* In debug mode you may want vlc to dump a core instead of staying stuck */
    sdl_flags |= SDL_INIT_NOPARACHUTE;
    /* Initialize library */
    if (SDL_Init(sdl_flags) < 0) {
        vlc_mutex_unlock(&sdl_lock);
        msg_Err(vd, "cannot initialize SDL (%s)", SDL_GetError());
        free(sys);
        return VLC_EGENERIC;
    }
    vlc_mutex_unlock(&sdl_lock);
    /* Translate keys into unicode */
    SDL_EnableUNICODE(1);
    /* Get the desktop resolution */
    /* FIXME: SDL has a problem with virtual desktop */
    sys->desktop_width  = SDL_GetVideoInfo()->current_w;
    sys->desktop_height = SDL_GetVideoInfo()->current_h;
    /* */
    video_format_t fmt;
    video_format_ApplyRotation(&fmt, &vd->fmt);
    /* */
    vout_display_info_t info = vd->info;
    /* Set main window's size */
    int display_width;
    int display_height;
    if (vd->cfg->is_fullscreen) {
        display_width  = sys->desktop_width;
        display_height = sys->desktop_height;
    } else {
        display_width  = vd->cfg->display.width;
        display_height = vd->cfg->display.height;
    }
    /* Initialize flags and cursor */
    sys->display_flags = SDL_ANYFORMAT | SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF;
    sys->display_flags |= vd->cfg->is_fullscreen ? SDL_FULLSCREEN : SDL_RESIZABLE;
    sys->display_bpp = SDL_VideoModeOK(display_width, display_height,
                                       16, sys->display_flags);
    if (sys->display_bpp == 0) {
        msg_Err(vd, "no video mode available");
        goto error;
    }
    vout_display_DeleteWindow(vd, NULL);
    sys->display = SDL_SetVideoMode(display_width, display_height,
                                    sys->display_bpp, sys->display_flags);
    if (!sys->display) {
        msg_Err(vd, "cannot set video mode");
        goto error;
    }
    /* We keep the surface locked forever */
    SDL_LockSurface(sys->display);
    /* */
    vlc_fourcc_t forced_chroma = 0;
    char *psz_chroma = var_InheritString(vd, "sdl-chroma");
    if (psz_chroma) {
        forced_chroma = vlc_fourcc_GetCodecFromString(VIDEO_ES, psz_chroma);
        if (forced_chroma)
            msg_Dbg(vd, "Forcing chroma to 0x%.8x (%4.4s)",
                    forced_chroma, (const char*)&forced_chroma);
        free(psz_chroma);
    }
    /* Try to open an overlay if requested */
    sys->overlay = NULL;
    const bool is_overlay = var_InheritBool(vd, "overlay");
    if (is_overlay) {
        static const struct
        {
            vlc_fourcc_t vlc;
            uint32_t     sdl;
        } vlc_to_sdl[] = {
            { VLC_CODEC_YV12, SDL_YV12_OVERLAY },
            { VLC_CODEC_I420, SDL_IYUV_OVERLAY },
            { VLC_CODEC_YUYV, SDL_YUY2_OVERLAY },
            { VLC_CODEC_UYVY, SDL_UYVY_OVERLAY },
            { VLC_CODEC_YVYU, SDL_YVYU_OVERLAY },
            { 0, 0 }
        };
        const vlc_fourcc_t forced_chromas[] = {
            forced_chroma, 0
        };
        const vlc_fourcc_t *fallback_chromas =
            vlc_fourcc_GetYUVFallback(fmt.i_chroma);
        const vlc_fourcc_t *chromas = forced_chroma ? forced_chromas : fallback_chromas;
        for (int pass = forced_chroma ? 1 : 0; pass < 2 && !sys->overlay; pass++) {
            for (int i = 0; chromas[i] != 0; i++) {
                const vlc_fourcc_t vlc = chromas[i];
                uint32_t sdl = 0;
                for (int j = 0; vlc_to_sdl[j].vlc != 0 && !sdl; j++) {
                    if (vlc_to_sdl[j].vlc == vlc)
                        sdl = vlc_to_sdl[j].sdl;
                }
                if (!sdl)
                    continue;
                sys->overlay = SDL_CreateYUVOverlay(fmt.i_width, fmt.i_height,
                                                    sdl, sys->display);
                if (sys->overlay && !sys->overlay->hw_overlay && pass == 0) {
                    /* Ignore non hardware overlay surface in first pass */
                    SDL_FreeYUVOverlay(sys->overlay);
                    sys->overlay = NULL;
                }
                if (sys->overlay) {
                    /* We keep the surface locked forever */
                    SDL_LockYUVOverlay(sys->overlay);
                    fmt.i_chroma = vlc;
                    sys->is_uv_swapped = vlc_fourcc_AreUVPlanesSwapped(fmt.i_chroma,
                                                                       vd->fmt.i_chroma);
                    if (sys->is_uv_swapped)
                        fmt.i_chroma = vd->fmt.i_chroma;
                    break;
                }
            }
        }
    } else {
        msg_Warn(vd, "SDL overlay disabled by the user");
    }
    /* */
    vout_display_cfg_t place_cfg = *vd->cfg;
    place_cfg.display.width  = display_width;
    place_cfg.display.height = display_height;
    vout_display_PlacePicture(&sys->place, &vd->source, &place_cfg, !sys->overlay);
    /* If no overlay, fallback to software output */
    if (!sys->overlay) {
        /* */
        switch (sys->display->format->BitsPerPixel) {
        case 8:
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
            msg_Err(vd, "unknown screen depth %i",
                    sys->display->format->BitsPerPixel);
            goto error;
        }
        /* All we have is an RGB image with square pixels */
        fmt.i_width  = display_width;
        fmt.i_height = display_height;
        fmt.i_rmask = sys->display->format->Rmask;
        fmt.i_gmask = sys->display->format->Gmask;
        fmt.i_bmask = sys->display->format->Bmask;
        info.has_pictures_invalid = true;
    }
    if (vd->cfg->display.title)
        SDL_WM_SetCaption(vd->cfg->display.title,
                          vd->cfg->display.title);
    else if (!sys->overlay)
        SDL_WM_SetCaption(VOUT_TITLE " (software RGB SDL output)",
                          VOUT_TITLE " (software RGB SDL output)");
    else if (sys->overlay->hw_overlay)
        SDL_WM_SetCaption(VOUT_TITLE " (hardware YUV SDL output)",
                          VOUT_TITLE " (hardware YUV SDL output)");
    else
        SDL_WM_SetCaption(VOUT_TITLE " (software YUV SDL output)",
                          VOUT_TITLE " (software YUV SDL output)");
    /* Setup events */
    SDL_EventState(SDL_KEYUP, SDL_IGNORE);               /* ignore keys up */
    /* Setup vout_display now that everything is fine */
    vd->fmt = fmt;
    vd->info = info;
    vd->pool    = Pool;
    vd->prepare = NULL;
    vd->display = PictureDisplay;
    vd->control = Control;
    vd->manage  = Manage;
    /* */
    vout_display_SendEventDisplaySize(vd, display_width, display_height, vd->cfg->is_fullscreen);
    return VLC_SUCCESS;
error:
    msg_Err(vd, "cannot set up SDL (%s)", SDL_GetError());
    if (sys->display) {
        SDL_UnlockSurface(sys->display);
        SDL_FreeSurface(sys->display);
    }
    vlc_mutex_lock(&sdl_lock);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    vlc_mutex_unlock(&sdl_lock);
    free(sys);
    return VLC_EGENERIC;
}
