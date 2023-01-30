 */
static int oldmovie_film_scratch_effect( filter_t *p_filter, picture_t *p_pic_out )
{
    filter_sys_t *p_sys = p_filter->p_sys;
#define SCRATCH_GENERATOR_PERIOD ( TIME_UNIT_PER_S * 2 )
#define SCRATCH_DURATION         ( TIME_UNIT_PER_S * 1 / 2)
    /* generate new scratch */
    if ( p_sys->i_scratch_trigger <= p_sys->i_cur_time ) {
        for ( uint32_t i_s = 0; i_s < MAX_SCRATCH; i_s++ )
            if ( p_sys->p_scratch[i_s] == NULL ) {
                /* allocate data */
                p_sys->p_scratch[i_s] = calloc( 1, sizeof(scratch_t) );
                if ( unlikely( !p_sys->p_scratch[i_s] ) )
                    return VLC_ENOMEM;
                /* set random parameters */
                p_sys->p_scratch[i_s]->i_offset = ( ( (unsigned)vlc_mrand48() )
                                                % __MAX( p_sys->i_width[Y_PLANE] - 10, 1 ) )
                                                + 5;
                p_sys->p_scratch[i_s]->i_width  = ( ( (unsigned)vlc_mrand48() )
                                                % __MAX( p_sys->i_width[Y_PLANE] / 500, 1 ) )
                                                + 1;
                p_sys->p_scratch[i_s]->i_intensity = (unsigned) vlc_mrand48() % 50 + 10;
                p_sys->p_scratch[i_s]->i_stop_trigger = p_sys->i_cur_time
                                                      + (uint64_t) vlc_mrand48() % SCRATCH_DURATION
                                                      + SCRATCH_DURATION / 2;
                break;
            }
        p_sys->i_scratch_trigger = p_sys->i_cur_time
                                 + ( (uint64_t)vlc_mrand48() ) % SCRATCH_GENERATOR_PERIOD
                                 + SCRATCH_GENERATOR_PERIOD / 2;
    }
    /* manage and apply current scratch */
    for ( uint32_t i_s = 0; i_s < MAX_SCRATCH; i_s++ )
        if ( p_sys->p_scratch[i_s] ) {
            /* remove outdated scratch */
            if ( p_sys->p_scratch[i_s]->i_stop_trigger <= p_sys->i_cur_time ) {
                FREENULL( p_sys->p_scratch[i_s] );
                continue;
            }
            /* otherwise apply scratch */
            for ( int32_t i_y = 0; i_y < p_pic_out->p[Y_PLANE].i_visible_lines; i_y++ )
                for ( int32_t i_x = p_sys->p_scratch[i_s]->i_offset;
                      i_x < __MIN(p_sys->p_scratch[i_s]->i_offset
                      + p_sys->p_scratch[i_s]->i_width, p_sys->i_width[Y_PLANE] );
                      i_x++ )
                    DARKEN_PIXEL( i_x, i_y, p_sys->p_scratch[i_s]->i_intensity,
                                  &p_pic_out->p[Y_PLANE] );
        }
    return VLC_SUCCESS;
}
