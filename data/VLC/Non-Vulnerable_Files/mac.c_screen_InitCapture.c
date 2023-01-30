};
int screen_InitCapture(demux_t *p_demux)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    screen_data_t *p_data;
    CGLError returnedError;
    p_sys->p_data = p_data = calloc(1, sizeof(screen_data_t));
    if (!p_data)
        return VLC_ENOMEM;
    /* fetch the screen we should capture */
    p_data->display_id = kCGDirectMainDisplay;
    p_data->rate = var_InheritFloat (p_demux, "screen-fps");
    unsigned int displayCount = 0;
    returnedError = CGGetOnlineDisplayList(0, NULL, &displayCount);
    if (!returnedError) {
        CGDirectDisplayID *ids;
        ids = (CGDirectDisplayID *)malloc(displayCount * sizeof(CGDirectDisplayID));
        returnedError = CGGetOnlineDisplayList(displayCount, ids, &displayCount);
        if (!returnedError) {
            if (p_sys->i_display_id > 0) {
                for (unsigned int i = 0; i < displayCount; i++) {
                    if (p_sys->i_display_id == ids[i]) {
                        p_data->display_id = ids[i];
                        break;
                    }
                }
            } else if (p_sys->i_screen_index > 0 && p_sys->i_screen_index <= displayCount)
                p_data->display_id = ids[p_sys->i_screen_index - 1];
        }
        free(ids);
    }
    /* Get the device context for the whole screen */
    CGRect rect = CGDisplayBounds(p_data->display_id);
    p_data->screen_left = rect.origin.x;
    p_data->screen_top = rect.origin.y;
    p_data->screen_width = rect.size.width;
    p_data->screen_height = rect.size.height;
    p_data->width = p_sys->i_width;
    p_data->height = p_sys->i_height;
    if (p_data->width <= 0 || p_data->height <= 0) {
        p_data->width = p_data->screen_width;
        p_data->height = p_data->screen_height;
    }
    /* setup format */
    es_format_Init(&p_sys->fmt, VIDEO_ES, VLC_CODEC_RGB32);
    p_sys->fmt.video.i_visible_width   =
    p_sys->fmt.video.i_width           = rect.size.width;
    p_sys->fmt.video.i_visible_height  =
    p_sys->fmt.video.i_height          = rect.size.height;
    p_sys->fmt.video.i_bits_per_pixel  = 32;
    p_sys->fmt.video.i_chroma          = VLC_CODEC_RGB32;
    p_sys->fmt.video.i_rmask           = 0x00ff0000;
    p_sys->fmt.video.i_gmask           = 0x0000ff00;
    p_sys->fmt.video.i_bmask           = 0x000000ff;
    p_sys->fmt.video.i_frame_rate      = 1000 * p_data->rate;
    p_sys->fmt.video.i_frame_rate_base = 1000;
    p_sys->fmt.video.i_sar_num         =
    p_sys->fmt.video.i_sar_den         = 1;
    return VLC_SUCCESS;
}
