void ff_draw_color(FFDrawContext *draw, FFDrawColor *color, const uint8_t rgba[4])
{
    unsigned i;
    uint8_t rgba_map[4];
    if (rgba != color->rgba)
        memcpy(color->rgba, rgba, sizeof(color->rgba));
    if ((draw->desc->flags & AV_PIX_FMT_FLAG_RGB) &&
        ff_fill_rgba_map(rgba_map, draw->format) >= 0) {
        if (draw->nb_planes == 1) {
        for (i = 0; i < 4; i++)
            color->comp[0].u8[rgba_map[i]] = rgba[i];
        } else {
            for (i = 0; i < 4; i++)
                color->comp[rgba_map[i]].u8[0] = rgba[i];
        }
    } else if (draw->nb_planes == 3 || draw->nb_planes == 4) {
        
        color->comp[0].u8[0] = RGB_TO_Y_CCIR(rgba[0], rgba[1], rgba[2]);
        color->comp[1].u8[0] = RGB_TO_U_CCIR(rgba[0], rgba[1], rgba[2], 0);
        color->comp[2].u8[0] = RGB_TO_V_CCIR(rgba[0], rgba[1], rgba[2], 0);
        color->comp[3].u8[0] = rgba[3];
    } else if (draw->format == AV_PIX_FMT_GRAY8 || draw->format == AV_PIX_FMT_GRAY8A) {
        color->comp[0].u8[0] = RGB_TO_Y_CCIR(rgba[0], rgba[1], rgba[2]);
        color->comp[1].u8[0] = rgba[3];
    } else {
        av_log(NULL, AV_LOG_WARNING,
               "Color conversion not implemented for %s\n", draw->desc->name);
        memset(color, 128, sizeof(*color));
    }
}
