}
bool vout_ManageDisplay(vout_display_t *vd, bool allow_reset_pictures)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    vout_display_Manage(vd);
    /* Handle mouse timeout */
    const mtime_t date = mdate();
    bool  hide_mouse = false;
    vlc_mutex_lock(&osys->lock);
    if (!osys->mouse.is_hidden &&
        osys->mouse.last_moved + osys->mouse.hide_timeout < date) {
        osys->mouse.is_hidden = hide_mouse = true;
    } else if (osys->mouse.ch_activity) {
        osys->mouse.is_hidden = false;
    }
    osys->mouse.ch_activity = false;
    vlc_mutex_unlock(&osys->lock);
    if (hide_mouse) {
        if (!vd->info.has_hide_mouse) {
            msg_Dbg(vd, "auto hiding mouse cursor");
            vout_display_Control(vd, VOUT_DISPLAY_HIDE_MOUSE);
        }
        vout_SendEventMouseHidden(osys->vout);
    }
    bool reset_render = false;
    for (;;) {
        vlc_mutex_lock(&osys->lock);
        bool ch_fullscreen  = osys->ch_fullscreen;
        bool is_fullscreen  = osys->is_fullscreen;
        osys->ch_fullscreen = false;
        bool ch_wm_state  = osys->ch_wm_state;
        unsigned wm_state  = osys->wm_state;
        osys->ch_wm_state = false;
        bool ch_display_size       = osys->ch_display_size;
        int  display_width         = osys->display_width;
        int  display_height        = osys->display_height;
        bool display_is_fullscreen = osys->display_is_fullscreen;
        bool display_is_forced     = osys->display_is_forced;
        osys->ch_display_size = false;
        bool reset_pictures;
        if (allow_reset_pictures) {
            reset_pictures = osys->reset_pictures;
            osys->reset_pictures = false;
        } else {
            reset_pictures = false;
        }
        vlc_mutex_unlock(&osys->lock);
        if (!ch_fullscreen &&
            !ch_display_size &&
            !reset_pictures &&
            !osys->ch_display_filled &&
            !osys->ch_zoom &&
            !ch_wm_state &&
            !osys->ch_sar &&
            !osys->ch_crop) {
            if (!osys->cfg.is_fullscreen && osys->fit_window != 0) {
                VoutDisplayFitWindow(vd, osys->fit_window == -1);
                osys->fit_window = 0;
                continue;
            }
            break;
        }
        /* */
        if (ch_fullscreen) {
            vout_display_cfg_t cfg = osys->cfg;
            cfg.is_fullscreen  = is_fullscreen;
            cfg.display.width  = cfg.is_fullscreen ? 0 : osys->width_saved;
            cfg.display.height = cfg.is_fullscreen ? 0 : osys->height_saved;
            if (vout_display_Control(vd, VOUT_DISPLAY_CHANGE_FULLSCREEN, &cfg)) {
                msg_Err(vd, "Failed to set fullscreen");
                is_fullscreen = osys->cfg.is_fullscreen;
            } else if (!is_fullscreen) {
                vout_display_Control(vd, VOUT_DISPLAY_CHANGE_DISPLAY_SIZE, &cfg, true);
            }
            osys->cfg.is_fullscreen = is_fullscreen;
            /* */
            vout_SendEventFullscreen(osys->vout, osys->cfg.is_fullscreen);
        }
        /* */
        if (ch_display_size) {
            vout_display_cfg_t cfg = osys->cfg;
            cfg.display.width  = display_width;
            cfg.display.height = display_height;
            if (!cfg.is_fullscreen != !display_is_fullscreen ||
                vout_display_Control(vd, VOUT_DISPLAY_CHANGE_DISPLAY_SIZE, &cfg, display_is_forced)) {
                if (!display_is_forced)
                    msg_Err(vd, "Failed to resize display");
                /* We ignore the resized */
                display_width  = osys->cfg.display.width;
                display_height = osys->cfg.display.height;
            }
            osys->cfg.display.width  = display_width;
            osys->cfg.display.height = display_height;
            if (!display_is_fullscreen) {
                osys->width_saved  = display_width;
                osys->height_saved = display_height;
            }
        }
        /* */
        if (osys->ch_display_filled) {
            vout_display_cfg_t cfg = osys->cfg;
            cfg.is_display_filled = osys->is_display_filled;
            if (vout_display_Control(vd, VOUT_DISPLAY_CHANGE_DISPLAY_FILLED, &cfg)) {
                msg_Err(vd, "Failed to change display filled state");
                osys->is_display_filled = osys->cfg.is_display_filled;
            }
            osys->cfg.is_display_filled = osys->is_display_filled;
            osys->ch_display_filled = false;
            vout_SendEventDisplayFilled(osys->vout, osys->cfg.is_display_filled);
        }
        /* */
        if (osys->ch_zoom) {
            vout_display_cfg_t cfg = osys->cfg;
            cfg.zoom.num = osys->zoom.num;
            cfg.zoom.den = osys->zoom.den;
            if (10 * cfg.zoom.num <= cfg.zoom.den) {
                cfg.zoom.num = 1;
                cfg.zoom.den = 10;
            } else if (cfg.zoom.num >= 10 * cfg.zoom.den) {
                cfg.zoom.num = 10;
                cfg.zoom.den = 1;
            }
            if (vout_display_Control(vd, VOUT_DISPLAY_CHANGE_ZOOM, &cfg)) {
                msg_Err(vd, "Failed to change zoom");
                osys->zoom.num = osys->cfg.zoom.num;
                osys->zoom.den = osys->cfg.zoom.den;
            } else {
                osys->fit_window = -1;
            }
            osys->cfg.zoom.num = osys->zoom.num;
            osys->cfg.zoom.den = osys->zoom.den;
            osys->ch_zoom = false;
            vout_SendEventZoom(osys->vout, osys->cfg.zoom.num, osys->cfg.zoom.den);
        }
        /* */
        if (ch_wm_state) {
            if (vout_display_Control(vd, VOUT_DISPLAY_CHANGE_WINDOW_STATE, wm_state)) {
                msg_Err(vd, "Failed to set on top");
                wm_state = osys->wm_state;
            }
            osys->wm_state_initial = wm_state;
            /* */
            vout_SendEventOnTop(osys->vout, osys->wm_state_initial);
        }
        /* */
        if (osys->ch_sar) {
            video_format_t source = vd->source;
            if (osys->sar.num > 0 && osys->sar.den > 0) {
                source.i_sar_num = osys->sar.num;
                source.i_sar_den = osys->sar.den;
            } else {
                source.i_sar_num = osys->source.i_sar_num;
                source.i_sar_den = osys->source.i_sar_den;
            }
            if (vout_display_Control(vd, VOUT_DISPLAY_CHANGE_SOURCE_ASPECT, &source)) {
                /* There nothing much we can do. The only reason a vout display
                 * does not support it is because it need the core to add black border
                 * to the video for it.
                 * TODO add black borders ?
                 */
                msg_Err(vd, "Failed to change source AR");
                source = vd->source;
            } else if (!osys->fit_window) {
                osys->fit_window = 1;
            }
            vd->source = source;
            osys->sar.num = source.i_sar_num;
            osys->sar.den = source.i_sar_den;
            osys->ch_sar  = false;
            /* */
            if (osys->sar.num == osys->source.i_sar_num &&
                osys->sar.den == osys->source.i_sar_den)
            {
                vout_SendEventSourceAspect(osys->vout, 0, 0);
            }
            else
            {
                unsigned dar_num, dar_den;
                vlc_ureduce( &dar_num, &dar_den,
                             osys->sar.num * vd->source.i_visible_width,
                             osys->sar.den * vd->source.i_visible_height,
                             65536);
                vout_SendEventSourceAspect(osys->vout, dar_num, dar_den);
            }
            /* If a crop ratio is requested, recompute the parameters */
            if (osys->crop.num > 0 && osys->crop.den > 0)
                osys->ch_crop = true;
        }
        /* */
        if (osys->ch_crop) {
            video_format_t source = vd->source;
            unsigned crop_num = osys->crop.num;
            unsigned crop_den = osys->crop.den;
            if (crop_num > 0 && crop_den > 0) {
                video_format_t fmt = osys->source;
                fmt.i_sar_num = source.i_sar_num;
                fmt.i_sar_den = source.i_sar_den;
                VoutDisplayCropRatio(&osys->crop.left,  &osys->crop.top,
                                     &osys->crop.right, &osys->crop.bottom,
                                     &fmt, crop_num, crop_den);
            }
            const int right_max  = osys->source.i_x_offset + osys->source.i_visible_width;
            const int bottom_max = osys->source.i_y_offset + osys->source.i_visible_height;
            int left   = VLC_CLIP((int)osys->source.i_x_offset + osys->crop.left,
                                0, right_max - 1);
            int top    = VLC_CLIP((int)osys->source.i_y_offset + osys->crop.top,
                                0, bottom_max - 1);
            int right, bottom;
            if (osys->crop.right <= 0)
                right = (int)(osys->source.i_x_offset + osys->source.i_visible_width) + osys->crop.right;
            else
                right = (int)osys->source.i_x_offset + osys->crop.right;
            right = VLC_CLIP(right, left + 1, right_max);
            if (osys->crop.bottom <= 0)
                bottom = (int)(osys->source.i_y_offset + osys->source.i_visible_height) + osys->crop.bottom;
            else
                bottom = (int)osys->source.i_y_offset + osys->crop.bottom;
            bottom = VLC_CLIP(bottom, top + 1, bottom_max);
            source.i_x_offset       = left;
            source.i_y_offset       = top;
            source.i_visible_width  = right - left;
            source.i_visible_height = bottom - top;
            video_format_Print(VLC_OBJECT(vd), "SOURCE ", &osys->source);
            video_format_Print(VLC_OBJECT(vd), "CROPPED", &source);
            if (vout_display_Control(vd, VOUT_DISPLAY_CHANGE_SOURCE_CROP, &source)) {
                msg_Err(vd, "Failed to change source crop TODO implement crop at core");
                source = vd->source;
                crop_num = osys->crop_saved.num;
                crop_den = osys->crop_saved.den;
                /* FIXME implement cropping in the core if not supported by the
                 * vout module (easy)
                 */
            } else if (!osys->fit_window) {
                osys->fit_window = 1;
            }
            vd->source = source;
            osys->crop.left   = source.i_x_offset - osys->source.i_x_offset;
            osys->crop.top    = source.i_y_offset - osys->source.i_y_offset;
            /* FIXME for right/bottom we should keep the 'type' border vs window */
            osys->crop.right  = (source.i_x_offset + source.i_visible_width) -
                                (osys->source.i_x_offset + osys->source.i_visible_width);
            osys->crop.bottom = (source.i_y_offset + source.i_visible_height) -
                                (osys->source.i_y_offset + osys->source.i_visible_height);
            osys->crop.num    = crop_num;
            osys->crop.den    = crop_den;
            osys->ch_crop = false;
            vout_SendEventSourceCrop(osys->vout,
                                     osys->crop.num, osys->crop.den,
                                     osys->crop.left, osys->crop.top,
                                     -osys->crop.right, -osys->crop.bottom);
        }
        /* */
        if (reset_pictures) {
            if (vout_display_Control(vd, VOUT_DISPLAY_RESET_PICTURES)) {
                /* FIXME what to do here ? */
                msg_Err(vd, "Failed to reset pictures (probably fatal)");
            }
            reset_render = true;
        }
    }
    if (reset_render)
        VoutDisplayResetRender(vd);
    return reset_render;
}
