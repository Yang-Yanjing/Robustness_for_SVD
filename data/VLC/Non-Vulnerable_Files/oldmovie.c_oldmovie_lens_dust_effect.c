 */
static int oldmovie_lens_dust_effect( filter_t *p_filter, picture_t *p_pic_out ) {
    filter_sys_t *p_sys = p_filter->p_sys;
    /* generate new dust */
    if ( p_sys->i_dust_trigger <= p_sys->i_cur_time ) {
        for ( uint32_t i_d = 0; i_d < MAX_DUST; i_d++ )
            if ( p_sys->p_dust[i_d] == NULL ) {
                /* allocate data */
                p_sys->p_dust[i_d] = calloc( 1, sizeof(dust_t) );
                if ( unlikely( !p_sys->p_dust[i_d] ) )
                    return VLC_ENOMEM;
                /* set random parameters */
                oldmovie_define_dust_location( p_filter, p_sys->p_dust[i_d] );
                p_sys->p_dust[i_d]->i_width = MOD( (int32_t)vlc_mrand48(), 5 ) + 1;
                p_sys->p_dust[i_d]->i_intensity = (unsigned)vlc_mrand48() % 30 + 30;
                break;
            }
        p_sys->i_dust_trigger = p_sys->i_cur_time
                              + (uint64_t)vlc_mrand48() % DUST_GENERATOR_PERIOD
                              + DUST_GENERATOR_PERIOD / 2;
    }
    /* manage and apply current dust */
    for ( uint32_t i_d = 0; i_d < MAX_DUST; i_d++ )
        if ( p_sys->p_dust[i_d] ) {
            /* remove outdated ones */
            if ( p_sys->p_dust[i_d]->i_stop_trigger <= p_sys->i_cur_time ) {
                /* select between moving or removing dust */
                if ( (unsigned)vlc_mrand48() % 2 == 0 )
                    /* move dust */
                    oldmovie_define_dust_location( p_filter, p_sys->p_dust[i_d] );
                else {
                    /* remove dust */
                    FREENULL( p_sys->p_dust[i_d] );
                    continue;
                }
            }
            /* draw dust */
            for ( int32_t i_y = -p_sys->p_dust[i_d]->i_width + 1; i_y < p_sys->p_dust[i_d]->i_width; i_y++ )
                for ( int32_t i_x = -p_sys->p_dust[i_d]->i_width + 1; i_x < p_sys->p_dust[i_d]->i_width; i_x++ )
                    if ( i_x * i_x + i_y * i_y <= p_sys->p_dust[i_d]->i_width * p_sys->p_dust[i_d]->i_width )
                        CHECK_N_DARKEN_PIXEL( i_x + p_sys->p_dust[i_d]->i_x,
                                              i_y + p_sys->p_dust[i_d]->i_y,
                                              p_sys->p_dust[i_d]->i_intensity,
                                              &p_pic_out->p[Y_PLANE] );
        }
    return VLC_SUCCESS;
}
