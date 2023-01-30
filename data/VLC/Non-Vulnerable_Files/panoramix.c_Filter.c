 */
static int Filter( video_splitter_t *p_splitter, picture_t *pp_dst[], picture_t *p_src )
{
    video_splitter_sys_t *p_sys = p_splitter->p_sys;
    if( video_splitter_NewPicture( p_splitter, pp_dst ) )
    {
        picture_Release( p_src );
        return VLC_EGENERIC;
    }
    for( int y = 0; y < p_sys->i_row; y++ )
    {
        for( int x = 0; x < p_sys->i_col; x++ )
        {
            const panoramix_output_t *p_output = &p_sys->pp_output[x][y];
            if( !p_output->b_active )
                continue;
            /* */
            picture_t *p_dst = pp_dst[p_output->i_output];
            /* */
            picture_CopyProperties( p_dst, p_src );
            /* */
            for( int i_plane = 0; i_plane < p_src->i_planes; i_plane++ )
            {
                const int i_div_w = p_sys->p_chroma->pi_div_w[i_plane];
                const int i_div_h = p_sys->p_chroma->pi_div_h[i_plane];
                if( !i_div_w || !i_div_h )
                    continue;
                const plane_t *p_srcp = &p_src->p[i_plane];
                const plane_t *p_dstp = &p_dst->p[i_plane];
                /* */
                panoramix_filter_t filter;
                filter.black.i_right  = p_output->filter.black.i_right / i_div_w;
                filter.black.i_left   = p_output->filter.black.i_left / i_div_w;
                filter.black.i_top    = p_output->filter.black.i_top / i_div_h;
                filter.black.i_bottom = p_output->filter.black.i_bottom / i_div_h;
                filter.attenuate.i_right  = p_output->filter.attenuate.i_right / i_div_w;
                filter.attenuate.i_left   = p_output->filter.attenuate.i_left / i_div_w;
                filter.attenuate.i_top    = p_output->filter.attenuate.i_top / i_div_h;
                filter.attenuate.i_bottom = p_output->filter.attenuate.i_bottom / i_div_h;
                /* */
                const int i_x = p_output->i_src_x/i_div_w;
                const int i_y = p_output->i_src_y/i_div_h;
                assert( p_sys->p_chroma->b_planar );
                FilterPlanar( p_dstp->p_pixels, p_dstp->i_pitch,
                              &p_srcp->p_pixels[i_y * p_srcp->i_pitch + i_x * p_srcp->i_pixel_pitch], p_srcp->i_pitch,
                              p_output->i_src_width/i_div_w, p_output->i_src_height/i_div_h,
                              p_sys->p_chroma->pi_black[i_plane],
                              &filter,
                              p_sys->p_lut[i_plane],
                              p_sys->lambdav[i_plane],
                              p_sys->lambdah[i_plane] );
            }
        }
    }
    picture_Release( p_src );
    return VLC_SUCCESS;
}
