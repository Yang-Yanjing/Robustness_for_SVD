}
static int RenderYUVA(filter_t *p_filter, subpicture_region_t *p_region,
                       CFMutableAttributedStringRef p_attrString)
{
    offscreen_bitmap_t *p_offScreen = NULL;
    unsigned      i_textblock_height = 0;
    unsigned i_width = p_filter->fmt_out.video.i_visible_width;
    unsigned i_height = p_filter->fmt_out.video.i_visible_height;
    unsigned i_text_align = p_region->i_align & 0x3;
    if (!p_attrString) {
        msg_Err(p_filter, "Invalid argument to RenderYUVA");
        return VLC_EGENERIC;
    }
    p_offScreen = Compose(i_text_align, p_attrString,
                           i_width, i_height, &i_textblock_height);
    if (!p_offScreen) {
        msg_Err(p_filter, "No offscreen buffer");
        return VLC_EGENERIC;
    }
    uint8_t *p_dst_y,*p_dst_u,*p_dst_v,*p_dst_a;
    video_format_t fmt;
    int i_offset;
    unsigned i_pitch;
    uint8_t i_y, i_u, i_v; // YUV values, derived from incoming RGB
    // Create a new subpicture region
    memset(&fmt, 0, sizeof(video_format_t));
    fmt.i_chroma = VLC_CODEC_YUVA;
    fmt.i_width = fmt.i_visible_width = i_width;
    fmt.i_height = fmt.i_visible_height = __MIN(i_height, i_textblock_height + VERTICAL_MARGIN * 2);
    fmt.i_x_offset = fmt.i_y_offset = 0;
    fmt.i_sar_num = 1;
    fmt.i_sar_den = 1;
    p_region->p_picture = picture_NewFromFormat(&fmt);
    if (!p_region->p_picture) {
        free(p_offScreen->p_data);
        free(p_offScreen);
        return VLC_EGENERIC;
    }
    p_region->fmt = fmt;
    p_dst_y = p_region->p_picture->Y_PIXELS;
    p_dst_u = p_region->p_picture->U_PIXELS;
    p_dst_v = p_region->p_picture->V_PIXELS;
    p_dst_a = p_region->p_picture->A_PIXELS;
    i_pitch = p_region->p_picture->A_PITCH;
    i_offset = (i_height + VERTICAL_MARGIN < fmt.i_height) ? VERTICAL_MARGIN *i_pitch : 0 ;
    for (unsigned y = 0; y < fmt.i_height; y++) {
        for (unsigned x = 0; x < fmt.i_width; x++) {
            int i_alpha = p_offScreen->p_data[ y * p_offScreen->i_bytesPerRow + x * p_offScreen->i_bytesPerPixel     ];
            int i_red   = p_offScreen->p_data[ y * p_offScreen->i_bytesPerRow + x * p_offScreen->i_bytesPerPixel + 1 ];
            int i_green = p_offScreen->p_data[ y * p_offScreen->i_bytesPerRow + x * p_offScreen->i_bytesPerPixel + 2 ];
            int i_blue  = p_offScreen->p_data[ y * p_offScreen->i_bytesPerRow + x * p_offScreen->i_bytesPerPixel + 3 ];
            i_y = (uint8_t)__MIN(abs(2104 * i_red  + 4130 * i_green +
                              802 * i_blue + 4096 + 131072) >> 13, 235);
            i_u = (uint8_t)__MIN(abs(-1214 * i_red  + -2384 * i_green +
                             3598 * i_blue + 4096 + 1048576) >> 13, 240);
            i_v = (uint8_t)__MIN(abs(3598 * i_red + -3013 * i_green +
                              -585 * i_blue + 4096 + 1048576) >> 13, 240);
            p_dst_y[ i_offset + x ] = i_y;
            p_dst_u[ i_offset + x ] = i_u;
            p_dst_v[ i_offset + x ] = i_v;
            p_dst_a[ i_offset + x ] = i_alpha;
        }
        i_offset += i_pitch;
    }
    free(p_offScreen->p_data);
    free(p_offScreen);
    return VLC_SUCCESS;
}
