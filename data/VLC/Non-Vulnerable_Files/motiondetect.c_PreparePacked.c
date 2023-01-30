}
static int PreparePacked( filter_t *p_filter, picture_t *p_inpic, int *pi_pix_offset )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    const video_format_t *p_fmt = &p_filter->fmt_in.video;
    int i_y_offset, i_u_offset, i_v_offset;
    if( GetPackedYuvOffsets( p_fmt->i_chroma,
                             &i_y_offset, &i_u_offset, &i_v_offset ) )
    {
        msg_Warn( p_filter, "Unsupported input chroma (%4.4s)",
                  (char*)&p_fmt->i_chroma );
        return VLC_EGENERIC;
    }
    *pi_pix_offset = i_y_offset;
    /* Substract all planes at once */
    uint8_t *p_oldpix   = p_sys->p_old->p[Y_PLANE].p_pixels;
    const int i_old_pitch = p_sys->p_old->p[Y_PLANE].i_pitch;
    const uint8_t *p_inpix = p_inpic->p[Y_PLANE].p_pixels;
    const int i_src_pitch = p_inpic->p[Y_PLANE].i_pitch;
    for( unsigned y = 0; y < p_fmt->i_height; y++ )
    {
        for( unsigned x = 0; x < p_fmt->i_width; x+=2 )
        {
            int d;
            d = abs( p_inpix[y*i_src_pitch+2*x+i_u_offset] - p_oldpix[y*i_old_pitch+2*x+i_u_offset] ) +
                abs( p_inpix[y*i_src_pitch+2*x+i_v_offset] - p_oldpix[y*i_old_pitch+2*x+i_v_offset] );
            for( int i = 0; i < 2; i++ )
                p_sys->p_buf2[y*p_fmt->i_width+x+i] =
                    abs( p_inpix[y*i_src_pitch+2*(x+i)+i_y_offset] - p_oldpix[y*i_old_pitch+2*(x+i)+i_y_offset] ) + d;
        }
    }
    return VLC_SUCCESS;
}
