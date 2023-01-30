VLC_TARGET
static void I420_cyuv( filter_t *p_filter, picture_t *p_source,
                                           picture_t *p_dest )
{
    uint8_t *p_line1 = p_dest->p->p_pixels +
                       p_dest->p->i_visible_lines * p_dest->p->i_pitch
                       + p_dest->p->i_pitch;
    uint8_t *p_line2 = p_dest->p->p_pixels +
                       p_dest->p->i_visible_lines * p_dest->p->i_pitch;
    uint8_t *p_y1, *p_y2 = p_source->Y_PIXELS;
    uint8_t *p_u = p_source->U_PIXELS;
    uint8_t *p_v = p_source->V_PIXELS;
    int i_x, i_y;
    const int i_source_margin = p_source->p[0].i_pitch
                                 - p_source->p[0].i_visible_pitch;
    const int i_source_margin_c = p_source->p[1].i_pitch
                                 - p_source->p[1].i_visible_pitch;
    const int i_dest_margin = p_dest->p->i_pitch
                               - p_dest->p->i_visible_pitch;
#if !defined(MODULE_NAME_IS_i420_yuy2_sse2)
    for( i_y = p_filter->fmt_in.video.i_height / 2 ; i_y-- ; )
    {
        p_line1 -= 3 * p_dest->p->i_pitch;
        p_line2 -= 3 * p_dest->p->i_pitch;
        p_y1 = p_y2;
        p_y2 += p_source->p[Y_PLANE].i_pitch;
        for( i_x = p_filter->fmt_in.video.i_width / 8 ; i_x-- ; )
        {
#if !defined (MODULE_NAME_IS_i420_yuy2_mmx)
            C_YUV420_UYVY( );
            C_YUV420_UYVY( );
            C_YUV420_UYVY( );
            C_YUV420_UYVY( );
#else
            MMX_CALL( MMX_YUV420_UYVY );
#endif
        }
        for( i_x = ( p_filter->fmt_in.video.i_width % 8 ) / 2; i_x-- ; )
        {
            C_YUV420_UYVY( );
        }
        p_y1 += i_source_margin;
        p_y2 += i_source_margin;
        p_u += i_source_margin_c;
        p_v += i_source_margin_c;
        p_line1 += i_dest_margin;
        p_line2 += i_dest_margin;
    }
#if defined (MODULE_NAME_IS_i420_yuy2_mmx)
    /* re-enable FPU registers */
    MMX_END;
#endif
#else // defined(MODULE_NAME_IS_i420_yuy2_sse2)
    /*
    ** SSE2 128 bits fetch/store instructions are faster
    ** if memory access is 16 bytes aligned
    */
    if( 0 == (15 & (p_source->p[Y_PLANE].i_pitch|p_dest->p->i_pitch|
        ((intptr_t)p_line2|(intptr_t)p_y2))) )
    {
        /* use faster SSE2 aligned fetch and store */
        for( i_y = p_filter->fmt_in.video.i_height / 2 ; i_y-- ; )
        {
            p_line1 = p_line2;
            p_line2 += p_dest->p->i_pitch;
            p_y1 = p_y2;
            p_y2 += p_source->p[Y_PLANE].i_pitch;
            for( i_x = p_filter->fmt_in.video.i_width / 16 ; i_x-- ; )
            {
                SSE2_CALL( SSE2_YUV420_UYVY_ALIGNED );
            }
            for( i_x = ( p_filter->fmt_in.video.i_width % 16 ) / 2; i_x-- ; )
            {
                C_YUV420_UYVY( );
            }
            p_y1 += i_source_margin;
            p_y2 += i_source_margin;
            p_u += i_source_margin_c;
            p_v += i_source_margin_c;
            p_line1 += i_dest_margin;
            p_line2 += i_dest_margin;
        }
    }
    else
    {
        /* use slower SSE2 unaligned fetch and store */
        for( i_y = p_filter->fmt_in.video.i_height / 2 ; i_y-- ; )
        {
            p_line1 = p_line2;
            p_line2 += p_dest->p->i_pitch;
            p_y1 = p_y2;
            p_y2 += p_source->p[Y_PLANE].i_pitch;
            for( i_x = p_filter->fmt_in.video.i_width / 16 ; i_x-- ; )
            {
                SSE2_CALL( SSE2_YUV420_UYVY_UNALIGNED );
            }
            for( i_x = ( p_filter->fmt_in.video.i_width % 16 ) / 2; i_x-- ; )
            {
                C_YUV420_UYVY( );
            }
            p_y1 += i_source_margin;
            p_y2 += i_source_margin;
            p_u += i_source_margin_c;
            p_v += i_source_margin_c;
            p_line1 += i_dest_margin;
            p_line2 += i_dest_margin;
        }
    }
    /* make sure all SSE2 stores are visible thereafter */
    SSE2_END;
#endif // defined(MODULE_NAME_IS_i420_yuy2_sse2)
}
