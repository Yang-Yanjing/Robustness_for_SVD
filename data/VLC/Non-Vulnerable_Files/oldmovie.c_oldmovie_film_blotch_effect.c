 */
static void oldmovie_film_blotch_effect( filter_t *p_filter, picture_t *p_pic_out )
{
    filter_sys_t *p_sys = p_filter->p_sys;
#define BLOTCH_GENERATOR_PERIOD ( TIME_UNIT_PER_S * 5 )
    /* generate blotch */
    if ( p_sys->i_blotch_trigger <= p_sys->i_cur_time ) {
        /* set random parameters */
        int32_t i_bx =        (unsigned)vlc_mrand48() % p_sys->i_width[Y_PLANE];
        int32_t i_by =        (unsigned)vlc_mrand48() % p_sys->i_height[Y_PLANE];
        int32_t i_width =     (unsigned)vlc_mrand48() % __MAX( 1, p_sys->i_width[Y_PLANE] / 10 ) + 1;
        int32_t i_intensity = (unsigned)vlc_mrand48() % 50 + 20;
        if ( (unsigned)vlc_mrand48() & 0x01 ) {
            /* draw dark blotch */
            for ( int32_t i_y = -i_width + 1; i_y < i_width; i_y++ )
                for ( int32_t i_x = -i_width + 1; i_x < i_width; i_x++ )
                    if ( i_x * i_x + i_y * i_y <= i_width * i_width )
                        CHECK_N_DARKEN_PIXEL( i_x + i_bx, i_y + i_by,
                                              i_intensity, &p_pic_out->p[Y_PLANE] );
        } else {
            /* draw light blotch */
            for ( int32_t i_y = -i_width+1; i_y < i_width; i_y++ )
                for ( int32_t i_x = -i_width+1; i_x < i_width; i_x++ )
                    if ( i_x * i_x + i_y * i_y <= i_width * i_width )
                        CHECK_N_LIGHTEN_PIXEL( i_x + i_bx, i_y + i_by,
                                               i_intensity, &p_pic_out->p[Y_PLANE] );
        }
        p_sys->i_blotch_trigger = p_sys->i_cur_time
                                + (uint64_t)vlc_mrand48() % BLOTCH_GENERATOR_PERIOD
                                + BLOTCH_GENERATOR_PERIOD / 2;
    }
}
