VLC_TARGET
static void I422_UYVY( filter_t *p_filter, picture_t *p_source,
                                           picture_t *p_dest )
{
    uint8_t *p_line = p_dest->p->p_pixels;
    uint8_t *p_y = p_source->Y_PIXELS;
    uint8_t *p_u = p_source->U_PIXELS;
    uint8_t *p_v = p_source->V_PIXELS;
    int i_x, i_y;
    const int i_source_margin = p_source->p[0].i_pitch
                                 - p_source->p[0].i_visible_pitch
                                 - p_filter->fmt_in.video.i_x_offset;
    const int i_source_margin_c = p_source->p[1].i_pitch
                                 - p_source->p[1].i_visible_pitch
                                 - ( p_filter->fmt_in.video.i_x_offset );
    const int i_dest_margin = p_dest->p->i_pitch
                               - p_dest->p->i_visible_pitch
                               - ( p_filter->fmt_out.video.i_x_offset * 2 );
#if defined (MODULE_NAME_IS_i422_yuy2_sse2)
    if( 0 == (15 & (p_source->p[Y_PLANE].i_pitch|p_dest->p->i_pitch|
        ((intptr_t)p_line|(intptr_t)p_y))) )
    {
        /* use faster SSE2 aligned fetch and store */
        for( i_y = (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height) ; i_y-- ; )
        {
            for( i_x = (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) / 16 ; i_x-- ; )
            {
                SSE2_CALL( SSE2_YUV422_UYVY_ALIGNED );
            }
            for( i_x = ( (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) % 16 ) / 2; i_x-- ; )
            {
                C_YUV422_UYVY( p_line, p_y, p_u, p_v );
            }
            p_y += i_source_margin;
            p_u += i_source_margin_c;
            p_v += i_source_margin_c;
            p_line += i_dest_margin;
        }
    }
    else {
        /* use slower SSE2 unaligned fetch and store */
        for( i_y = (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height) ; i_y-- ; )
        {
            for( i_x = (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) / 16 ; i_x-- ; )
            {
                SSE2_CALL( SSE2_YUV422_UYVY_UNALIGNED );
            }
            for( i_x = ( (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) % 16 ) / 2; i_x-- ; )
            {
                C_YUV422_UYVY( p_line, p_y, p_u, p_v );
            }
            p_y += i_source_margin;
            p_u += i_source_margin_c;
            p_v += i_source_margin_c;
            p_line += i_dest_margin;
        }
    }
    SSE2_END;
#else
    for( i_y = (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height) ; i_y-- ; )
    {
        for( i_x = (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) / 8 ; i_x-- ; )
        {
#if defined (MODULE_NAME_IS_i422_yuy2)
            C_YUV422_UYVY( p_line, p_y, p_u, p_v );
            C_YUV422_UYVY( p_line, p_y, p_u, p_v );
            C_YUV422_UYVY( p_line, p_y, p_u, p_v );
            C_YUV422_UYVY( p_line, p_y, p_u, p_v );
#elif defined (MODULE_NAME_IS_i422_yuy2_mmx)
            MMX_CALL( MMX_YUV422_UYVY );
#endif
        }
        for( i_x = ( (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) % 8 ) / 2; i_x-- ; )
        {
            C_YUV422_UYVY( p_line, p_y, p_u, p_v );
        }
        p_y += i_source_margin;
        p_u += i_source_margin_c;
        p_v += i_source_margin_c;
        p_line += i_dest_margin;
    }
#if defined (MODULE_NAME_IS_i422_yuy2_mmx)
    MMX_END;
#endif
#endif
}