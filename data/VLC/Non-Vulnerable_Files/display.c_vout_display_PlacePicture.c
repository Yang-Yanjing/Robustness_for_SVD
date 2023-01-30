/* */
void vout_display_PlacePicture(vout_display_place_t *place,
                               const video_format_t *source,
                               const vout_display_cfg_t *cfg,
                               bool do_clipping)
{
    /* */
    memset(place, 0, sizeof(*place));
    if (cfg->display.width <= 0 || cfg->display.height <= 0)
        return;
    /* */
    unsigned display_width;
    unsigned display_height;
    video_format_t source_rot;
    video_format_ApplyRotation(&source_rot, source);
    source = &source_rot;
    if (cfg->is_display_filled) {
        display_width  = cfg->display.width;
        display_height = cfg->display.height;
    } else {
        vout_display_cfg_t cfg_tmp = *cfg;
        cfg_tmp.display.width  = 0;
        cfg_tmp.display.height = 0;
        vout_display_GetDefaultDisplaySize(&display_width, &display_height,
                                           source, &cfg_tmp);
        if (do_clipping) {
            display_width  = __MIN(display_width,  cfg->display.width);
            display_height = __MIN(display_height, cfg->display.height);
        }
    }
    const unsigned width  = source->i_visible_width;
    const unsigned height = source->i_visible_height;
    /* Compute the height if we use the width to fill up display_width */
    const int64_t scaled_height = (int64_t)height * display_width  * cfg->display.sar.num * source->i_sar_den / width  / source->i_sar_num / cfg->display.sar.den;
    /* And the same but switching width/height */
    const int64_t scaled_width  = (int64_t)width  * display_height * cfg->display.sar.den * source->i_sar_num / height / source->i_sar_den / cfg->display.sar.num;
    /* We keep the solution that avoid filling outside the display */
    if (scaled_width <= cfg->display.width) {
        place->width  = scaled_width;
        place->height = display_height;
    } else {
        place->width  = display_width;
        place->height = scaled_height;
    }
    /*  Compute position */
    switch (cfg->align.horizontal) {
    case VOUT_DISPLAY_ALIGN_LEFT:
        place->x = 0;
        break;
    case VOUT_DISPLAY_ALIGN_RIGHT:
        place->x = cfg->display.width - place->width;
        break;
    default:
        place->x = ((int)cfg->display.width - (int)place->width) / 2;
        break;
    }
    switch (cfg->align.vertical) {
    case VOUT_DISPLAY_ALIGN_TOP:
        place->y = 0;
        break;
    case VOUT_DISPLAY_ALIGN_BOTTOM:
        place->y = cfg->display.height - place->height;
        break;
    default:
        place->y = ((int)cfg->display.height - (int)place->height) / 2;
        break;
    }
}
