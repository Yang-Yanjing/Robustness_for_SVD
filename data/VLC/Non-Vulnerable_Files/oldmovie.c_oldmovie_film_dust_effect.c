 */
static void oldmovie_film_dust_effect( filter_t *p_filter, picture_t *p_pic_out ) {
#define ONESHOT_DUST_RATIO 1000
    filter_sys_t *p_sys = p_filter->p_sys;
    for ( int32_t i_dust = 0;
          i_dust < p_sys->i_width[Y_PLANE] * p_sys->i_height[Y_PLANE] / ONESHOT_DUST_RATIO;
          i_dust++)
        if ( (unsigned)vlc_mrand48() % 5 < 3 )
            DARKEN_PIXEL( (unsigned)vlc_mrand48() % p_sys->i_width[Y_PLANE],
                          (unsigned)vlc_mrand48() % p_sys->i_height[Y_PLANE],
                          150, &p_pic_out->p[Y_PLANE] );
        else
            LIGHTEN_PIXEL( (unsigned)vlc_mrand48() % p_sys->i_width[Y_PLANE],
                           (unsigned)vlc_mrand48() % p_sys->i_height[Y_PLANE],
                           50, &p_pic_out->p[Y_PLANE] );
}
