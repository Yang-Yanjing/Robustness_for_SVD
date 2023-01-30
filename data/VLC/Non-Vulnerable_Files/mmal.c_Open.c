static void show_background(vout_display_t *vd, bool enable);
static int Open(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
    uint32_t buffer_pitch, buffer_height;
    vout_display_place_t place;
    MMAL_DISPLAYREGION_T display_region;
    uint32_t offsets[3];
    MMAL_STATUS_T status;
    int ret = VLC_SUCCESS;
    unsigned i;
    sys = calloc(1, sizeof(struct vout_display_sys_t));
    if (!sys)
        return VLC_ENOMEM;
    vd->sys = sys;
    sys->layer = var_InheritInteger(vd, MMAL_LAYER_NAME);
    bcm_host_init();
    vd->info.has_hide_mouse = true;
    sys->opaque = vd->fmt.i_chroma == VLC_CODEC_MMAL_OPAQUE;
    if (!sys->opaque)
        vd->fmt.i_chroma = VLC_CODEC_I420;
    vd->fmt.i_sar_num = vd->source.i_sar_num;
    vd->fmt.i_sar_den = vd->source.i_sar_den;
    buffer_pitch = align(vd->fmt.i_width, 32);
    buffer_height = align(vd->fmt.i_height, 16);
    sys->buffer_size = 3 * buffer_pitch * buffer_height / 2;
    status = mmal_component_create(MMAL_COMPONENT_DEFAULT_VIDEO_RENDERER, &sys->component);
    if (status != MMAL_SUCCESS) {
        msg_Err(vd, "Failed to create MMAL component %s (status=%"PRIx32" %s)",
                        MMAL_COMPONENT_DEFAULT_VIDEO_RENDERER, status, mmal_status_to_string(status));
        ret = VLC_EGENERIC;
        goto out;
    }
    sys->component->control->userdata = (struct MMAL_PORT_USERDATA_T *)vd;
    status = mmal_port_enable(sys->component->control, control_port_cb);
    if (status != MMAL_SUCCESS) {
        msg_Err(vd, "Failed to enable control port %s (status=%"PRIx32" %s)",
                        sys->component->control->name, status, mmal_status_to_string(status));
        ret = VLC_EGENERIC;
        goto out;
    }
    sys->input = sys->component->input[0];
    sys->input->userdata = (struct MMAL_PORT_USERDATA_T *)vd;
    if (sys->opaque)
        sys->input->format->encoding = MMAL_ENCODING_OPAQUE;
    else
        sys->input->format->encoding = MMAL_ENCODING_I420;
    sys->input->format->es->video.width = vd->fmt.i_width;
    sys->input->format->es->video.height = vd->fmt.i_height;
    sys->input->format->es->video.crop.x = 0;
    sys->input->format->es->video.crop.y = 0;
    sys->input->format->es->video.crop.width = vd->fmt.i_width;
    sys->input->format->es->video.crop.height = vd->fmt.i_height;
    sys->input->format->es->video.par.num = vd->source.i_sar_num;
    sys->input->format->es->video.par.den = vd->source.i_sar_den;
    status = mmal_port_format_commit(sys->input);
    if (status != MMAL_SUCCESS) {
        msg_Err(vd, "Failed to commit format for input port %s (status=%"PRIx32" %s)",
                        sys->input->name, status, mmal_status_to_string(status));
        ret = VLC_EGENERIC;
        goto out;
    }
    sys->input->buffer_size = sys->input->buffer_size_recommended;
    vout_display_PlacePicture(&place, &vd->source, vd->cfg, false);
    display_region.hdr.id = MMAL_PARAMETER_DISPLAYREGION;
    display_region.hdr.size = sizeof(MMAL_DISPLAYREGION_T);
    display_region.fullscreen = MMAL_FALSE;
    display_region.src_rect.x = vd->fmt.i_x_offset;
    display_region.src_rect.y = vd->fmt.i_y_offset;
    display_region.src_rect.width = vd->fmt.i_visible_width;
    display_region.src_rect.height = vd->fmt.i_visible_height;
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
        ret = VLC_EGENERIC;
        goto out;
    }
    offsets[0] = 0;
    for (i = 0; i < 3; ++i) {
        sys->planes[i].i_lines = buffer_height;
        sys->planes[i].i_pitch = buffer_pitch;
        sys->planes[i].i_visible_lines = vd->fmt.i_visible_height;
        sys->planes[i].i_visible_pitch = vd->fmt.i_visible_width;
        if (i > 0) {
            offsets[i] = offsets[i - 1] + sys->planes[i - 1].i_pitch * sys->planes[i - 1].i_lines;
            sys->planes[i].i_lines /= 2;
            sys->planes[i].i_pitch /= 2;
            sys->planes[i].i_visible_lines /= 2;
            sys->planes[i].i_visible_pitch /= 2;
        }
        sys->planes[i].p_pixels = (uint8_t *)offsets[i];
    }
    vlc_mutex_init(&sys->buffer_mutex);
    vlc_cond_init(&sys->buffer_cond);
    vlc_mutex_init(&sys->manage_mutex);
    vd->pool = vd_pool;
    vd->display = vd_display;
    vd->control = vd_control;
    vd->manage = vd_manage;
    vc_tv_register_callback(tvservice_cb, vd);
    if (query_resolution(vd, &sys->display_width, &sys->display_height) >= 0) {
        vout_display_SendEventDisplaySize(vd, sys->display_width, sys->display_height,
                        vd->cfg->is_fullscreen);
    } else {
        sys->display_width = vd->cfg->display.width;
        sys->display_height = vd->cfg->display.height;
    }
    sys->dmx_handle = vc_dispmanx_display_open(0);
    vd->info.subpicture_chromas = subpicture_chromas;
out:
    if (ret != VLC_SUCCESS)
        Close(object);
    return ret;
}
