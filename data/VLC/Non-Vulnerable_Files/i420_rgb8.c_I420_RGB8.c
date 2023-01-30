 *****************************************************************************/
void I420_RGB8( filter_t *p_filter, picture_t *p_src, picture_t *p_dest )
{
    /* We got this one from the old arguments */
    uint8_t *p_pic = (uint8_t*)p_dest->p->p_pixels;
    uint8_t *p_y   = p_src->Y_PIXELS;
    uint8_t *p_u   = p_src->U_PIXELS;
    uint8_t *p_v   = p_src->V_PIXELS;
    bool  b_hscale;                         /* horizontal scaling type */
    int i_vscale;                                 /* vertical scaling type */
    unsigned int i_x, i_y;                /* horizontal and vertical indexes */
    unsigned int i_real_y;                                          /* y % 4 */
    int          i_right_margin;
    int          i_scale_count;                      /* scale modulo counter */
    unsigned int i_chroma_width = (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) / 2;/* chroma width */
    /* Lookup table */
    uint8_t *        p_lookup = p_filter->p_sys->p_base;
    /* Offset array pointer */
    int *       p_offset_start = p_filter->p_sys->p_offset;
    int *       p_offset;
    const int i_source_margin = p_src->p[0].i_pitch
                                 - p_src->p[0].i_visible_pitch
                                 - p_filter->fmt_in.video.i_x_offset;
    const int i_source_margin_c = p_src->p[1].i_pitch
                                 - p_src->p[1].i_visible_pitch
                                 - ( p_filter->fmt_in.video.i_x_offset / 2 );
    /* The dithering matrices */
    static const int dither10[4] = {  0x0,  0x8,  0x2,  0xa };
    static const int dither11[4] = {  0xc,  0x4,  0xe,  0x6 };
    static const int dither12[4] = {  0x3,  0xb,  0x1,  0x9 };
    static const int dither13[4] = {  0xf,  0x7,  0xd,  0x5 };
    static const int dither20[4] = {  0x0, 0x10,  0x4, 0x14 };
    static const int dither21[4] = { 0x18,  0x8, 0x1c,  0xc };
    static const int dither22[4] = {  0x6, 0x16,  0x2, 0x12 };
    static const int dither23[4] = { 0x1e,  0xe, 0x1a,  0xa };
    SetOffset( (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width),
               (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height),
               (p_filter->fmt_out.video.i_x_offset + p_filter->fmt_out.video.i_visible_width),
               (p_filter->fmt_out.video.i_y_offset + p_filter->fmt_out.video.i_visible_height),
               &b_hscale, &i_vscale, p_offset_start );
    i_right_margin = p_dest->p->i_pitch - p_dest->p->i_visible_pitch;
    /*
     * Perform conversion
     */
    i_scale_count = ( i_vscale == 1 ) ?
                    (p_filter->fmt_out.video.i_y_offset + p_filter->fmt_out.video.i_visible_height) :
                    (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height);
    for( i_y = 0, i_real_y = 0; i_y < (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height); i_y++ )
    {
        /* Do horizontal and vertical scaling */
        SCALE_WIDTH_DITHER( 420 );
        SCALE_HEIGHT_DITHER( 420 );
    }
    p_y += i_source_margin;
    if( i_y % 2 )
    {
        p_u += i_source_margin_c;
        p_v += i_source_margin_c;
    }
}
