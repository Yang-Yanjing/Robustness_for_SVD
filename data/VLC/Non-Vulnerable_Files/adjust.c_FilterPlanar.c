 *****************************************************************************/
static picture_t *FilterPlanar( filter_t *p_filter, picture_t *p_pic )
{
    int pi_luma[256];
    int pi_gamma[256];
    picture_t *p_outpic;
    uint8_t *p_in, *p_in_end, *p_line_end;
    uint8_t *p_out;
    bool b_thres;
    double  f_hue;
    double  f_gamma;
    int32_t i_cont, i_lum;
    int i_sat, i_sin, i_cos, i_x, i_y;
    int i;
    filter_sys_t *p_sys = p_filter->p_sys;
    if( !p_pic ) return NULL;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    /* Get variables */
    vlc_mutex_lock( &p_sys->lock );
    i_cont = (int)( p_sys->f_contrast * 255 );
    i_lum = (int)( (p_sys->f_brightness - 1.0)*255 );
    f_hue = (float)( p_sys->i_hue * M_PI / 180 );
    i_sat = (int)( p_sys->f_saturation * 256 );
    f_gamma = 1.0 / p_sys->f_gamma;
    b_thres = p_sys->b_brightness_threshold;
    vlc_mutex_unlock( &p_sys->lock );
    /*
     * Threshold mode drops out everything about luma, contrast and gamma.
     */
    if( !b_thres )
    {
        /* Contrast is a fast but kludged function, so I put this gap to be
         * cleaner :) */
        i_lum += 128 - i_cont / 2;
        /* Fill the gamma lookup table */
        for( i = 0 ; i < 256 ; i++ )
        {
            pi_gamma[ i ] = clip_uint8_vlc( pow(i / 255.0, f_gamma) * 255.0);
        }
        /* Fill the luma lookup table */
        for( i = 0 ; i < 256 ; i++ )
        {
            pi_luma[ i ] = pi_gamma[clip_uint8_vlc( i_lum + i_cont * i / 256)];
        }
    }
    else
    {
        /*
         * We get luma as threshold value: the higher it is, the darker is
         * the image. Should I reverse this?
         */
        for( i = 0 ; i < 256 ; i++ )
        {
            pi_luma[ i ] = (i < i_lum) ? 0 : 255;
        }
        /*
         * Desaturates image to avoid that strange yellow halo...
         */
        i_sat = 0;
    }
    /*
     * Do the Y plane
     */
    p_in = p_pic->p[Y_PLANE].p_pixels;
    p_in_end = p_in + p_pic->p[Y_PLANE].i_visible_lines
                      * p_pic->p[Y_PLANE].i_pitch - 8;
    p_out = p_outpic->p[Y_PLANE].p_pixels;
    for( ; p_in < p_in_end ; )
    {
        p_line_end = p_in + p_pic->p[Y_PLANE].i_visible_pitch - 8;
        for( ; p_in < p_line_end ; )
        {
            /* Do 8 pixels at a time */
            *p_out++ = pi_luma[ *p_in++ ]; *p_out++ = pi_luma[ *p_in++ ];
            *p_out++ = pi_luma[ *p_in++ ]; *p_out++ = pi_luma[ *p_in++ ];
            *p_out++ = pi_luma[ *p_in++ ]; *p_out++ = pi_luma[ *p_in++ ];
            *p_out++ = pi_luma[ *p_in++ ]; *p_out++ = pi_luma[ *p_in++ ];
        }
        p_line_end += 8;
        for( ; p_in < p_line_end ; )
        {
            *p_out++ = pi_luma[ *p_in++ ];
        }
        p_in += p_pic->p[Y_PLANE].i_pitch
              - p_pic->p[Y_PLANE].i_visible_pitch;
        p_out += p_outpic->p[Y_PLANE].i_pitch
               - p_outpic->p[Y_PLANE].i_visible_pitch;
    }
    /*
     * Do the U and V planes
     */
    i_sin = sin(f_hue) * 256;
    i_cos = cos(f_hue) * 256;
    i_x = ( cos(f_hue) + sin(f_hue) ) * 32768;
    i_y = ( cos(f_hue) - sin(f_hue) ) * 32768;
    if ( i_sat > 256 )
    {
        /* Currently no errors are implemented in the function, if any are added
         * check them here */
        p_sys->pf_process_sat_hue_clip( p_pic, p_outpic, i_sin, i_cos, i_sat,
                                        i_x, i_y );
    }
    else
    {
        /* Currently no errors are implemented in the function, if any are added
         * check them here */
        p_sys->pf_process_sat_hue( p_pic, p_outpic, i_sin, i_cos, i_sat,
                                        i_x, i_y );
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}
