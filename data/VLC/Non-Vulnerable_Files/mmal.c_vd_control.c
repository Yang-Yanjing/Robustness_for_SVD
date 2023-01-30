}
static int vd_control(vout_display_t *vd, int query, va_list args)
{
    vout_display_sys_t *sys = vd->sys;
    vout_display_cfg_t cfg;
    const vout_display_cfg_t *tmp_cfg;
    video_format_t fmt;
    const video_format_t *tmp_fmt;
    int ret = VLC_EGENERIC;
    switch (query) {
        case VOUT_DISPLAY_HIDE_MOUSE:
        case VOUT_DISPLAY_CHANGE_WINDOW_STATE:
            ret = VLC_SUCCESS;
            break;
        case VOUT_DISPLAY_CHANGE_FULLSCREEN:
            tmp_cfg = va_arg(args, const vout_display_cfg_t *);
            vout_display_SendEventDisplaySize(vd, sys->display_width,
                            sys->display_height, tmp_cfg->is_fullscreen);
            ret = VLC_SUCCESS;
            break;
        case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE:
            tmp_cfg = va_arg(args, const vout_display_cfg_t *);
            if (tmp_cfg->display.width == sys->display_width &&
                            tmp_cfg->display.height == sys->display_height) {
                cfg = *vd->cfg;
                cfg.display.width = sys->display_width;
                cfg.display.height = sys->display_height;
                if (configure_display(vd, &cfg, NULL) >= 0)
                    ret = VLC_SUCCESS;
            }
            break;
        case VOUT_DISPLAY_CHANGE_DISPLAY_FILLED:
            cfg = *vd->cfg;
            tmp_cfg = va_arg(args, const vout_display_cfg_t *);
            cfg.is_display_filled = tmp_cfg->is_display_filled;
            if (configure_display(vd, &cfg, NULL) >= 0)
                ret = VLC_SUCCESS;
            break;
        case VOUT_DISPLAY_CHANGE_SOURCE_ASPECT:
            fmt = vd->fmt;
            tmp_fmt = va_arg(args, const video_format_t *);
            fmt.i_sar_num = tmp_fmt->i_sar_num;
            fmt.i_sar_den = tmp_fmt->i_sar_den;
            if (configure_display(vd, NULL, &fmt) >= 0)
                ret = VLC_SUCCESS;
            break;
        case VOUT_DISPLAY_CHANGE_SOURCE_CROP:
            fmt = vd->fmt;
            tmp_fmt = va_arg(args, const video_format_t *);
            fmt.i_x_offset = tmp_fmt->i_x_offset;
            fmt.i_y_offset = tmp_fmt->i_y_offset;
            fmt.i_visible_width = tmp_fmt->i_visible_width;
            fmt.i_visible_height = tmp_fmt->i_visible_height;
            if (configure_display(vd, NULL, &fmt) >= 0)
                ret = VLC_SUCCESS;
            break;
        case VOUT_DISPLAY_CHANGE_ZOOM:
        case VOUT_DISPLAY_RESET_PICTURES:
        case VOUT_DISPLAY_GET_OPENGL:
            msg_Warn(vd, "Unsupported control query %d", query);
            break;
        default:
            msg_Warn(vd, "Unknown control query %d", query);
            break;
    }
    return ret;
}
