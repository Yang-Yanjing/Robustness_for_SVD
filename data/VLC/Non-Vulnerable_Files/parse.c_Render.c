}
static void Render( decoder_t *p_dec, subpicture_t *p_spu,
                    subpicture_data_t *p_spu_data,
                    const spu_properties_t *p_spu_properties )
{
    uint8_t *p_p;
    int i_x, i_y, i_len, i_color, i_pitch;
    const uint16_t *p_source = p_spu_data->p_data;
    video_format_t fmt;
    video_palette_t palette;
    /* Create a new subpicture region */
    memset( &fmt, 0, sizeof(video_format_t) );
    fmt.i_chroma = VLC_CODEC_YUVP;
    fmt.i_sar_num = 0; /* 0 means use aspect ratio of background video */
    fmt.i_sar_den = 1;
    fmt.i_width = fmt.i_visible_width = p_spu_properties->i_width;
    fmt.i_height = fmt.i_visible_height = p_spu_properties->i_height -
        p_spu_data->i_y_top_offset - p_spu_data->i_y_bottom_offset;
    fmt.i_x_offset = fmt.i_y_offset = 0;
    fmt.p_palette = &palette;
    fmt.p_palette->i_entries = 4;
    for( i_x = 0; i_x < fmt.p_palette->i_entries; i_x++ )
    {
        fmt.p_palette->palette[i_x][0] = p_spu_data->pi_yuv[i_x][0];
        fmt.p_palette->palette[i_x][1] = p_spu_data->pi_yuv[i_x][1];
        fmt.p_palette->palette[i_x][2] = p_spu_data->pi_yuv[i_x][2];
        fmt.p_palette->palette[i_x][3] = p_spu_data->pi_alpha[i_x] * 0x11;
    }
    p_spu->p_region = subpicture_region_New( &fmt );
    if( !p_spu->p_region )
    {
        msg_Err( p_dec, "cannot allocate SPU region" );
        return;
    }
    p_spu->p_region->i_x = p_spu_properties->i_x;
    p_spu->p_region->i_y = p_spu_properties->i_y + p_spu_data->i_y_top_offset;
    p_p = p_spu->p_region->p_picture->p->p_pixels;
    i_pitch = p_spu->p_region->p_picture->p->i_pitch;
    /* Draw until we reach the bottom of the subtitle */
    for( i_y = 0; i_y < (int)fmt.i_height * i_pitch; i_y += i_pitch )
    {
        /* Draw until we reach the end of the line */
        for( i_x = 0 ; i_x < (int)fmt.i_width; i_x += i_len )
        {
            /* Get the RLE part, then draw the line */
            i_color = *p_source & 0x3;
            i_len = *p_source++ >> 2;
            memset( p_p + i_x + i_y, i_color, i_len );
        }
    }
}
