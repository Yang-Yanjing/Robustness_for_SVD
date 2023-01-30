 */
static int Control(vout_display_t *vd, int query, va_list args)
{
    vout_display_sys_t *sys = vd->sys;
    switch (query)
    {
    case VOUT_DISPLAY_HIDE_MOUSE:
        SDL_ShowCursor(0);
        return VLC_SUCCESS;
    case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE: {
        const vout_display_cfg_t *cfg = va_arg(args, const vout_display_cfg_t *);
        /* */
        sys->display = SDL_SetVideoMode(cfg->display.width,
                                        cfg->display.height,
                                        sys->display_bpp, sys->display_flags);
        if (!sys->display) {
            sys->display = SDL_SetVideoMode(vd->cfg->display.width,
                                            vd->cfg->display.height,
                                            sys->display_bpp, sys->display_flags);
            return VLC_EGENERIC;
        }
        if (sys->overlay)
            vout_display_PlacePicture(&sys->place, &vd->source, cfg, !sys->overlay);
        else
            vout_display_SendEventPicturesInvalid(vd);
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_CHANGE_FULLSCREEN: {
        vout_display_cfg_t cfg = *va_arg(args, const vout_display_cfg_t *);
        /* Fix flags */
        sys->display_flags &= ~(SDL_FULLSCREEN | SDL_RESIZABLE);
        sys->display_flags |= cfg.is_fullscreen ? SDL_FULLSCREEN : SDL_RESIZABLE;
        if (cfg.is_fullscreen) {
            cfg.display.width = sys->desktop_width;
            cfg.display.height = sys->desktop_height;
        }
        if (sys->overlay) {
            sys->display = SDL_SetVideoMode(cfg.display.width, cfg.display.height,
                                            sys->display_bpp, sys->display_flags);
            vout_display_PlacePicture(&sys->place, &vd->source, &cfg, !sys->overlay);
        }
        vout_display_SendEventDisplaySize(vd, cfg.display.width, cfg.display.height, cfg.is_fullscreen);
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_CHANGE_ZOOM:
    case VOUT_DISPLAY_CHANGE_DISPLAY_FILLED:
    case VOUT_DISPLAY_CHANGE_SOURCE_ASPECT: {
        const vout_display_cfg_t *cfg;
        const video_format_t *source;
        if (query == VOUT_DISPLAY_CHANGE_SOURCE_ASPECT) {
            source = va_arg(args, const video_format_t *);
            cfg = vd->cfg;
        } else {
            source = &vd->source;
            cfg = va_arg(args, const vout_display_cfg_t *);
        }
        if (sys->overlay) {
            sys->display = SDL_SetVideoMode(cfg->display.width, cfg->display.height,
                                            sys->display_bpp, sys->display_flags);
            vout_display_PlacePicture(&sys->place, source, cfg, !sys->overlay);
        } else {
            vout_display_SendEventPicturesInvalid(vd);
        }
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_RESET_PICTURES: {
        /* */
        assert(!sys->overlay);
        /* */
        if (sys->pool)
            picture_pool_Delete(sys->pool);
        sys->pool = NULL;
        vout_display_PlacePicture(&sys->place, &vd->source, vd->cfg, !sys->overlay);
        /* */
        vd->fmt.i_width  = sys->place.width;
        vd->fmt.i_height = sys->place.height;
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_CHANGE_SOURCE_CROP:
    case VOUT_DISPLAY_CHANGE_WINDOW_STATE:
        /* I don't think it is possible to support with SDL:
         * - crop
         * - on top
         */
        return VLC_EGENERIC;
    default:
        msg_Err(vd, "Unsupported query in vout display SDL");
        return VLC_EGENERIC;
    }
}
