#if defined (MODULE_NAME_IS_i420_yuy2)
static void I420_Y211( filter_t *p_filter, picture_t *p_source,
                                           picture_t *p_dest )
{
    uint8_t *p_line1, *p_line2 = p_dest->p->p_pixels;
    uint8_t *p_y1, *p_y2 = p_source->Y_PIXELS;
    uint8_t *p_u = p_source->U_PIXELS;
    uint8_t *p_v = p_source->V_PIXELS;
    int i_x, i_y;
    const int i_source_margin = p_source->p[0].i_pitch
                                 - p_source->p[0].i_visible_pitch
                                 - p_filter->fmt_in.video.i_x_offset;
    const int i_source_margin_c = p_source->p[1].i_pitch
                                 - p_source->p[1].i_visible_pitch
                                 - ( p_filter->fmt_in.video.i_x_offset / 2 );
    const int i_dest_margin = p_dest->p->i_pitch
                               - p_dest->p->i_visible_pitch
                               - ( p_filter->fmt_out.video.i_x_offset * 2 );
    for( i_y = (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height) / 2 ; i_y-- ; )
    {
        p_line1 = p_line2;
        p_line2 += p_dest->p->i_pitch;
        p_y1 = p_y2;
        p_y2 += p_source->p[Y_PLANE].i_pitch;
        for( i_x = (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) / 8 ; i_x-- ; )
        {
            C_YUV420_Y211( );
            C_YUV420_Y211( );
        }
        p_y1 += i_source_margin;
        p_y2 += i_source_margin;
        p_u += i_source_margin_c;
        p_v += i_source_margin_c;
        p_line1 += i_dest_margin;
        p_line2 += i_dest_margin;
    }
}
