 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    int i, j;
    uint8_t *restrict p_src = NULL;
    uint8_t *restrict p_out = NULL;
    int i_src_pitch;
    int i_out_pitch;
    int pix;
    const int v1 = -1;
    const int v2 = 3; /* 2^3 = 8 */
    const unsigned i_visible_lines = p_pic->p[Y_PLANE].i_visible_lines;
    const unsigned i_visible_pitch = p_pic->p[Y_PLANE].i_visible_pitch;
    const int sigma = var_GetFloat( p_filter, FILTER_PREFIX "sigma" ) * (1 << 20);
    if( !p_pic ) return NULL;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    /* process the Y plane */
    p_src = p_pic->p[Y_PLANE].p_pixels;
    p_out = p_outpic->p[Y_PLANE].p_pixels;
    i_src_pitch = p_pic->p[Y_PLANE].i_pitch;
    i_out_pitch = p_outpic->p[Y_PLANE].i_pitch;
    /* perform convolution only on Y plane. Avoid border line. */
    vlc_mutex_lock( &p_filter->p_sys->lock );
    memcpy(p_out, p_src, i_visible_pitch);
    for( i = 1; i < i_visible_lines - 1; i++ )
    {
        p_out[i * i_out_pitch] = p_src[i * i_src_pitch];
        for( j = 1; j < i_visible_pitch - 1; j++ )
        {
            pix = (p_src[(i - 1) * i_src_pitch + j - 1] * v1) +
                  (p_src[(i - 1) * i_src_pitch + j    ] * v1) +
                  (p_src[(i - 1) * i_src_pitch + j + 1] * v1) +
                  (p_src[(i    ) * i_src_pitch + j - 1] * v1) +
                  (p_src[(i    ) * i_src_pitch + j    ] << v2) +
                  (p_src[(i    ) * i_src_pitch + j + 1] * v1) +
                  (p_src[(i + 1) * i_src_pitch + j - 1] * v1) +
                  (p_src[(i + 1) * i_src_pitch + j    ] * v1) +
                  (p_src[(i + 1) * i_src_pitch + j + 1] * v1);
           pix = pix >= 0 ? clip(pix) : -clip(pix * -1);
           p_out[i * i_out_pitch + j] = clip( p_src[i * i_src_pitch + j]
                                              + ((pix * sigma) >> 20));
        }
        p_out[i * i_out_pitch + i_visible_pitch - 1] =
            p_src[i * i_src_pitch + i_visible_pitch - 1];
    }
    memcpy(&p_out[(i_visible_lines - 1) * i_out_pitch],
           &p_src[(i_visible_lines - 1) * i_src_pitch], i_visible_pitch);
    vlc_mutex_unlock( &p_filter->p_sys->lock );
    plane_CopyPixels( &p_outpic->p[U_PLANE], &p_pic->p[U_PLANE] );
    plane_CopyPixels( &p_outpic->p[V_PLANE], &p_pic->p[V_PLANE] );
    return CopyInfoAndRelease( p_outpic, p_pic );
}
