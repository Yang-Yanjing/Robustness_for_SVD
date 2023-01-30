 *****************************************************************************/
static void PreparePlanar( filter_t *p_filter, picture_t *p_inpic )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    const video_format_t *p_fmt = &p_filter->fmt_in.video;
    uint8_t *p_oldpix   = p_sys->p_old->p[Y_PLANE].p_pixels;
    const int i_old_pitch = p_sys->p_old->p[Y_PLANE].i_pitch;
    const uint8_t *p_inpix = p_inpic->p[Y_PLANE].p_pixels;
    const int i_src_pitch = p_inpic->p[Y_PLANE].i_pitch;
    /**
     * Substract Y planes
     */
    for( unsigned y = 0; y < p_fmt->i_height; y++ )
    {
        for( unsigned x = 0; x < p_fmt->i_width; x++ )
            p_sys->p_buf2[y*p_fmt->i_width+x] = abs( p_inpix[y*i_src_pitch+x] - p_oldpix[y*i_old_pitch+x] );
    }
    int i_chroma_dx;
    int i_chroma_dy;
    switch( p_inpic->format.i_chroma )
    {
        case VLC_CODEC_I420:
        case VLC_CODEC_J420:
        case VLC_CODEC_YV12:
            i_chroma_dx = 2;
            i_chroma_dy = 2;
            break;
        case VLC_CODEC_I422:
        case VLC_CODEC_J422:
            i_chroma_dx = 2;
            i_chroma_dy = 1;
            break;
        default:
            msg_Warn( p_filter, "Not taking chroma into account" );
            return;
    }
    const uint8_t *p_inpix_u = p_inpic->p[U_PLANE].p_pixels;
    const uint8_t *p_inpix_v = p_inpic->p[V_PLANE].p_pixels;
    const int i_src_pitch_u = p_inpic->p[U_PLANE].i_pitch;
    const int i_src_pitch_v = p_inpic->p[V_PLANE].i_pitch;
    const uint8_t *p_oldpix_u = p_sys->p_old->p[U_PLANE].p_pixels;
    const uint8_t *p_oldpix_v = p_sys->p_old->p[V_PLANE].p_pixels;
    const int i_old_pitch_u = p_sys->p_old->p[U_PLANE].i_pitch;
    const int i_old_pitch_v = p_sys->p_old->p[V_PLANE].i_pitch;
    for( unsigned y = 0; y < p_fmt->i_height/i_chroma_dy; y++ )
    {
        for( unsigned x = 0; x < p_fmt->i_width/i_chroma_dx; x ++ )
        {
            const int d = abs( p_inpix_u[y*i_src_pitch_u+x] - p_oldpix_u[y*i_old_pitch_u+x] ) +
                          abs( p_inpix_v[y*i_src_pitch_v+x] - p_oldpix_v[y*i_old_pitch_v+x] );
            int i, j;
            for( j = 0; j < i_chroma_dy; j++ )
            {
                for( i = 0; i < i_chroma_dx; i++ )
                    p_sys->p_buf2[i_chroma_dy*p_fmt->i_width*j + i_chroma_dx*i] = d;
            }
        }
    }
}
