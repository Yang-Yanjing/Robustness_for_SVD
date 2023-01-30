 *****************************************************************************/
static int RenderYUVP( filter_t *p_filter, subpicture_region_t *p_region,
                       line_desc_t *p_line,
                       FT_BBox *p_bbox )
{
    VLC_UNUSED(p_filter);
    static const uint8_t pi_gamma[16] =
        {0x00, 0x52, 0x84, 0x96, 0xb8, 0xca, 0xdc, 0xee, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    uint8_t *p_dst;
    video_format_t fmt;
    int i, x, y, i_pitch;
    uint8_t i_y, i_u, i_v; /* YUV values, derived from incoming RGB */
    /* Create a new subpicture region */
    video_format_Init( &fmt, VLC_CODEC_YUVP );
    fmt.i_width          =
    fmt.i_visible_width  = p_bbox->xMax - p_bbox->xMin + 4;
    fmt.i_height         =
    fmt.i_visible_height = p_bbox->yMax - p_bbox->yMin + 4;
    assert( !p_region->p_picture );
    p_region->p_picture = picture_NewFromFormat( &fmt );
    if( !p_region->p_picture )
        return VLC_EGENERIC;
    fmt.p_palette = p_region->fmt.p_palette ? p_region->fmt.p_palette : malloc(sizeof(*fmt.p_palette));
    p_region->fmt = fmt;
    /* Calculate text color components
     * Only use the first color */
    int i_alpha = (p_line->p_character[0].i_color >> 24) & 0xff;
    YUVFromRGB( p_line->p_character[0].i_color, &i_y, &i_u, &i_v );
    /* Build palette */
    fmt.p_palette->i_entries = 16;
    for( i = 0; i < 8; i++ )
    {
        fmt.p_palette->palette[i][0] = 0;
        fmt.p_palette->palette[i][1] = 0x80;
        fmt.p_palette->palette[i][2] = 0x80;
        fmt.p_palette->palette[i][3] = pi_gamma[i];
        fmt.p_palette->palette[i][3] =
            (int)fmt.p_palette->palette[i][3] * i_alpha / 255;
    }
    for( i = 8; i < fmt.p_palette->i_entries; i++ )
    {
        fmt.p_palette->palette[i][0] = i * 16 * i_y / 256;
        fmt.p_palette->palette[i][1] = i_u;
        fmt.p_palette->palette[i][2] = i_v;
        fmt.p_palette->palette[i][3] = pi_gamma[i];
        fmt.p_palette->palette[i][3] =
            (int)fmt.p_palette->palette[i][3] * i_alpha / 255;
    }
    p_dst = p_region->p_picture->Y_PIXELS;
    i_pitch = p_region->p_picture->Y_PITCH;
    /* Initialize the region pixels */
    memset( p_dst, 0, i_pitch * p_region->fmt.i_height );
    for( ; p_line != NULL; p_line = p_line->p_next )
    {
        int i_align_left = 0;
        if( p_line->i_width < (int)fmt.i_visible_width )
        {
            if( (p_region->i_align & 0x3) == SUBPICTURE_ALIGN_RIGHT )
                i_align_left = ( fmt.i_visible_width - p_line->i_width );
            else if( (p_region->i_align & 0x3) != SUBPICTURE_ALIGN_LEFT )
                i_align_left = ( fmt.i_visible_width - p_line->i_width ) / 2;
        }
        int i_align_top = 0;
        for( i = 0; i < p_line->i_character_count; i++ )
        {
            const line_character_t *ch = &p_line->p_character[i];
            FT_BitmapGlyph p_glyph = ch->p_glyph;
            int i_glyph_y = i_align_top  - p_glyph->top  + p_bbox->yMax + p_line->i_base_line;
            int i_glyph_x = i_align_left + p_glyph->left - p_bbox->xMin;
            for( y = 0; y < p_glyph->bitmap.rows; y++ )
            {
                for( x = 0; x < p_glyph->bitmap.width; x++ )
                {
                    if( p_glyph->bitmap.buffer[y * p_glyph->bitmap.width + x] )
                        p_dst[(i_glyph_y + y) * i_pitch + (i_glyph_x + x)] =
                            (p_glyph->bitmap.buffer[y * p_glyph->bitmap.width + x] + 8)/16;
                }
            }
        }
    }
    /* Outlining (find something better than nearest neighbour filtering ?) */
    if( 1 )
    {
        uint8_t *p_dst = p_region->p_picture->Y_PIXELS;
        uint8_t *p_top = p_dst; /* Use 1st line as a cache */
        uint8_t left, current;
        for( y = 1; y < (int)fmt.i_height - 1; y++ )
        {
            if( y > 1 ) memcpy( p_top, p_dst, fmt.i_width );
            p_dst += p_region->p_picture->Y_PITCH;
            left = 0;
            for( x = 1; x < (int)fmt.i_width - 1; x++ )
            {
                current = p_dst[x];
                p_dst[x] = ( 8 * (int)p_dst[x] + left + p_dst[x+1] + p_top[x -1]+ p_top[x] + p_top[x+1] +
                             p_dst[x -1 + p_region->p_picture->Y_PITCH ] + p_dst[x + p_region->p_picture->Y_PITCH] + p_dst[x + 1 + p_region->p_picture->Y_PITCH]) / 16;
                left = current;
            }
        }
        memset( p_top, 0, fmt.i_width );
    }
    return VLC_SUCCESS;
}
