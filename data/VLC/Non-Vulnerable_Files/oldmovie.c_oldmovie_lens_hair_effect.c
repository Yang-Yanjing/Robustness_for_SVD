 */
static int oldmovie_lens_hair_effect( filter_t *p_filter, picture_t *p_pic_out ) {
    filter_sys_t *p_sys = p_filter->p_sys;
    /* generate new hair */
    if ( p_sys->i_hair_trigger <= p_sys->i_cur_time ) {
        for ( uint32_t i_h = 0; i_h < MAX_HAIR; i_h++ )
            if ( p_sys->p_hair[i_h] == NULL ) {
                /* allocate data */
                p_sys->p_hair[i_h] = calloc( 1, sizeof(hair_t) );
                if ( unlikely( !p_sys->p_hair[i_h] ) )
                    return VLC_ENOMEM;
                /* set random parameters */
                p_sys->p_hair[i_h]->i_length = (unsigned)vlc_mrand48()
                                             % ( p_sys->i_width[Y_PLANE] / 3 ) + 5;
                p_sys->p_hair[i_h]->i_curve  = MOD( (int32_t)vlc_mrand48(), 80 ) - 40;
                p_sys->p_hair[i_h]->i_width  = (unsigned)vlc_mrand48()
                                             % __MAX( 1, p_sys->i_width[Y_PLANE] / 1500 ) + 1;
                p_sys->p_hair[i_h]->i_intensity = (unsigned)vlc_mrand48() % 50 + 20;
                oldmovie_define_hair_location( p_filter, p_sys->p_hair[i_h] );
                break;
            }
        p_sys->i_hair_trigger = p_sys->i_cur_time
                              + (uint64_t)vlc_mrand48() % HAIR_GENERATOR_PERIOD
                              + HAIR_GENERATOR_PERIOD / 2;
    }
    /* manage and apply current hair */
    for ( uint32_t i_h = 0; i_h < MAX_HAIR; i_h++ )
        if ( p_sys->p_hair[i_h] ) {
            /* remove outdated ones */
            if ( p_sys->p_hair[i_h]->i_stop_trigger <= p_sys->i_cur_time ) {
                /* select between moving or removing hair */
                if ( (unsigned)vlc_mrand48() % 2 == 0 )
                    /* move hair */
                    oldmovie_define_hair_location( p_filter, p_sys->p_hair[i_h] );
                else {
                    /* remove hair */
                    FREENULL( p_sys->p_hair[i_h] );
                    continue;
                }
            }
            /* draw hair */
            double  f_base_x   = (double)p_sys->p_hair[i_h]->i_x;
            double  f_base_y   = (double)p_sys->p_hair[i_h]->i_y;
            for ( int32_t i_l = 0; i_l < p_sys->p_hair[i_h]->i_length; i_l++ ) {
                uint32_t i_current_rot = p_sys->p_hair[i_h]->i_rotation
                                       + p_sys->p_hair[i_h]->i_curve * i_l / 100;
                f_base_x += cos( (double)i_current_rot / 128.0 * M_PI );
                f_base_y += sin( (double)i_current_rot / 128.0 * M_PI );
                double f_current_x = f_base_x;
                double f_current_y = f_base_y;
                for ( int32_t i_w = 0; i_w < p_sys->p_hair[i_h]->i_width; i_w++ ) {
                    f_current_x += sin( (double)i_current_rot / 128.0 * M_PI );
                    f_current_y += cos( (double)i_current_rot / 128.0 * M_PI );
                    CHECK_N_DARKEN_PIXEL( (int32_t) f_current_x,
                                          (int32_t) f_current_y,
                                          p_sys->p_hair[i_h]->i_intensity,
                                          &p_pic_out->p[Y_PLANE] );
                }
            }
        }
    return VLC_SUCCESS;
}
