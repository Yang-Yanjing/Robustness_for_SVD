 *****************************************************************************/
void I420_RGB16( filter_t *p_filter, picture_t *p_src, picture_t *p_dest )
{
    /* We got this one from the old arguments */
    uint16_t *p_pic = (uint16_t*)p_dest->p->p_pixels;
    uint8_t  *p_y   = p_src->Y_PIXELS;
    uint8_t  *p_u   = p_src->U_PIXELS;
    uint8_t  *p_v   = p_src->V_PIXELS;
    bool  b_hscale;                         /* horizontal scaling type */
    unsigned int i_vscale;                          /* vertical scaling type */
    unsigned int i_x, i_y;                /* horizontal and vertical indexes */
    int         i_right_margin;
    int         i_rewind;
    int         i_scale_count;                       /* scale modulo counter */
    int         i_chroma_width = (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) / 2; /* chroma width */
    uint16_t *  p_pic_start;       /* beginning of the current line for copy */
    int         i_uval, i_vval;                           /* U and V samples */
    int         i_red, i_green, i_blue;          /* U and V modified samples */
    uint16_t *  p_yuv = p_filter->p_sys->p_rgb16;
    uint16_t *  p_ybase;                     /* Y dependant conversion table */
    /* Conversion buffer pointer */
    uint16_t *  p_buffer_start = (uint16_t*)p_filter->p_sys->p_buffer;
    uint16_t *  p_buffer;
    /* Offset array pointer */
    int *       p_offset_start = p_filter->p_sys->p_offset;
    int *       p_offset;
    const int i_source_margin = p_src->p[0].i_pitch
                                 - p_src->p[0].i_visible_pitch
                                 - p_filter->fmt_in.video.i_x_offset;
    const int i_source_margin_c = p_src->p[1].i_pitch
                                 - p_src->p[1].i_visible_pitch
                                 - ( p_filter->fmt_in.video.i_x_offset / 2 );
    i_right_margin = p_dest->p->i_pitch - p_dest->p->i_visible_pitch;
    i_rewind = (-(p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width)) & 7;
    /* Rule: when a picture of size (x1,y1) with aspect ratio r1 is rendered
     * on a picture of size (x2,y2) with aspect ratio r2, if x1 grows to x1'
     * then y1 grows to y1' = x1' * y2/x2 * r2/r1 */
    SetOffset( (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width),
               (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height),
               (p_filter->fmt_out.video.i_x_offset + p_filter->fmt_out.video.i_visible_width),
               (p_filter->fmt_out.video.i_y_offset + p_filter->fmt_out.video.i_visible_height),
               &b_hscale, &i_vscale, p_offset_start );
    /*
     * Perform conversion
     */
    i_scale_count = ( i_vscale == 1 ) ?
                    (p_filter->fmt_out.video.i_y_offset + p_filter->fmt_out.video.i_visible_height) :
                    (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height);
    for( i_y = 0; i_y < (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height); i_y++ )
    {
        p_pic_start = p_pic;
        p_buffer = b_hscale ? p_buffer_start : p_pic;
        for ( i_x = (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) / 8; i_x--; )
        {
            CONVERT_YUV_PIXEL(2);  CONVERT_Y_PIXEL(2);
            CONVERT_YUV_PIXEL(2);  CONVERT_Y_PIXEL(2);
            CONVERT_YUV_PIXEL(2);  CONVERT_Y_PIXEL(2);
            CONVERT_YUV_PIXEL(2);  CONVERT_Y_PIXEL(2);
        }
        /* Here we do some unaligned reads and duplicate conversions, but
         * at least we have all the pixels */
        if( i_rewind )
        {
            p_y -= i_rewind;
            p_u -= i_rewind >> 1;
            p_v -= i_rewind >> 1;
            p_buffer -= i_rewind;
            CONVERT_YUV_PIXEL(2);  CONVERT_Y_PIXEL(2);
            CONVERT_YUV_PIXEL(2);  CONVERT_Y_PIXEL(2);
            CONVERT_YUV_PIXEL(2);  CONVERT_Y_PIXEL(2);
            CONVERT_YUV_PIXEL(2);  CONVERT_Y_PIXEL(2);
        }
        SCALE_WIDTH;
        SCALE_HEIGHT( 420, 2 );
        p_y += i_source_margin;
        if( i_y % 2 )
        {
            p_u += i_source_margin_c;
            p_v += i_source_margin_c;
        }
    }
}