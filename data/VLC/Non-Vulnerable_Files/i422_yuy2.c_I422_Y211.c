#if defined (MODULE_NAME_IS_i422_yuy2)
static void I422_Y211( filter_t *p_filter, picture_t *p_source,
                                           picture_t *p_dest )
{
    uint8_t *p_line = p_dest->p->p_pixels + p_dest->p->i_visible_lines * p_dest->p->i_pitch;
    uint8_t *p_y = p_source->Y_PIXELS;
    uint8_t *p_u = p_source->U_PIXELS;
    uint8_t *p_v = p_source->V_PIXELS;
    int i_x, i_y;
    for( i_y = (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height) ; i_y-- ; )
    {
        for( i_x = (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) / 8 ; i_x-- ; )
        {
            C_YUV422_Y211( p_line, p_y, p_u, p_v );
            C_YUV422_Y211( p_line, p_y, p_u, p_v );
        }
    }
}
