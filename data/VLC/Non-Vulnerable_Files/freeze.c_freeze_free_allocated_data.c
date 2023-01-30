 */
static void freeze_free_allocated_data( filter_t *p_filter ) {
    filter_sys_t *p_sys = p_filter->p_sys;
    if (p_sys->pi_freezing_countdown)
        for ( int32_t i_r = 0; i_r < p_sys->i_height[Y_PLANE]; i_r++ )
            free( p_sys->pi_freezing_countdown[i_r] );
    FREENULL( p_sys->pi_freezing_countdown );
    if ( p_sys->pb_update_cache )
        for ( int32_t i_r = 0; i_r < p_sys->i_height[Y_PLANE]; i_r++ )
            free( p_sys->pb_update_cache[i_r] );
    FREENULL( p_sys->pb_update_cache );
    if ( p_sys->pi_freezed_picture )
        for ( int32_t i_p=0; i_p < p_sys->i_planes; i_p++ ) {
            for ( int32_t i_r=0; i_r < p_sys->i_height[i_p]; i_r++ )
                free( p_sys->pi_freezed_picture[i_p][i_r] );
            free( p_sys->pi_freezed_picture[i_p] );
            }
    FREENULL( p_sys->pi_freezed_picture );
    p_sys->i_planes = 0;
    FREENULL( p_sys->i_height );
    FREENULL( p_sys->i_width );
    FREENULL( p_sys->i_visible_pitch );
}
