 *****************************************************************************/
void RenderX( picture_t *p_outpic, picture_t *p_pic )
{
    int i_plane;
#if defined (CAN_COMPILE_MMXEXT)
    const bool mmxext = vlc_CPU_MMXEXT();
#endif
    /* Copy image and skip lines */
    for( i_plane = 0 ; i_plane < p_pic->i_planes ; i_plane++ )
    {
        const int i_mby = ( p_outpic->p[i_plane].i_visible_lines + 7 )/8 - 1;
        const int i_mbx = p_outpic->p[i_plane].i_visible_pitch/8;
        const int i_mody = p_outpic->p[i_plane].i_visible_lines - 8*i_mby;
        const int i_modx = p_outpic->p[i_plane].i_visible_pitch - 8*i_mbx;
        const int i_dst = p_outpic->p[i_plane].i_pitch;
        const int i_src = p_pic->p[i_plane].i_pitch;
        int y, x;
        for( y = 0; y < i_mby; y++ )
        {
            uint8_t *dst = &p_outpic->p[i_plane].p_pixels[8*y*i_dst];
            uint8_t *src = &p_pic->p[i_plane].p_pixels[8*y*i_src];
#ifdef CAN_COMPILE_MMXEXT
            if( mmxext )
                XDeintBand8x8MMXEXT( dst, i_dst, src, i_src, i_mbx, i_modx );
            else
#endif
                XDeintBand8x8C( dst, i_dst, src, i_src, i_mbx, i_modx );
        }
        /* Last line (C only)*/
        if( i_mody )
        {
            uint8_t *dst = &p_outpic->p[i_plane].p_pixels[8*y*i_dst];
            uint8_t *src = &p_pic->p[i_plane].p_pixels[8*y*i_src];
            for( x = 0; x < i_mbx; x++ )
            {
                XDeintNxN( dst, i_dst, src, i_src, 8, i_mody );
                dst += 8;
                src += 8;
            }
            if( i_modx )
                XDeintNxN( dst, i_dst, src, i_src, i_modx, i_mody );
        }
    }
#ifdef CAN_COMPILE_MMXEXT
    if( mmxext )
        emms();
#endif
}
