}
static int configure_display(vout_display_t *vd, const vout_display_cfg_t *cfg,
                const video_format_t *fmt)
{
    vout_display_sys_t *sys = vd->sys;
    vout_display_place_t place;
    MMAL_DISPLAYREGION_T display_region;
    MMAL_STATUS_T status;
    if (!cfg && !fmt)
        return -EINVAL;
    if (fmt) {
        sys->input->format->es->video.par.num = fmt->i_sar_num;
        sys->input->format->es->video.par.den = fmt->i_sar_den;
        status = mmal_port_format_commit(sys->input);
        if (status != MMAL_SUCCESS) {
            msg_Err(vd, "Failed to commit format for input port %s (status=%"PRIx32" %s)",
                            sys->input->name, status, mmal_status_to_string(status));
            return -EINVAL;
        }
    } else {
        fmt = &vd->fmt;
    }
    if (!cfg)
        cfg = vd->cfg;
    vout_display_PlacePicture(&place, fmt, cfg, false);
    display_region.hdr.id = MMAL_PARAMETER_DISPLAYREGION;
    display_region.hdr.size = sizeof(MMAL_DISPLAYREGION_T);
    display_region.fullscreen = MMAL_FALSE;
    display_region.src_rect.x = fmt->i_x_offset;
    display_region.src_rect.y = fmt->i_y_offset;
    display_region.src_rect.width = fmt->i_visible_width;
    display_region.src_rect.height = fmt->i_visible_height;
    display_region.dest_rect.x = place.x;
    display_region.dest_rect.y = place.y;
    display_region.dest_rect.width = place.width;
    display_region.dest_rect.height = place.height;
    display_region.layer = sys->layer;
    display_region.set = MMAL_DISPLAY_SET_FULLSCREEN | MMAL_DISPLAY_SET_SRC_RECT |
            MMAL_DISPLAY_SET_DEST_RECT | MMAL_DISPLAY_SET_LAYER;
    status = mmal_port_parameter_set(sys->input, &display_region.hdr);
    if (status != MMAL_SUCCESS) {
        msg_Err(vd, "Failed to set display region (status=%"PRIx32" %s)",
                        status, mmal_status_to_string(status));
        return -EINVAL;
    }
    show_background(vd, cfg->is_fullscreen);
    if (var_InheritBool(vd, MMAL_ADJUST_REFRESHRATE_NAME))
        adjust_refresh_rate(vd);
    if (fmt != &vd->fmt)
        memcpy(&vd->fmt, fmt, sizeof(video_format_t));
    return 0;
}
