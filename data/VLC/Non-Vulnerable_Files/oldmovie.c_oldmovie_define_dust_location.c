 */
static void oldmovie_define_dust_location( filter_t *p_filter, dust_t* ps_dust ) {
    filter_sys_t *p_sys = p_filter->p_sys;
    ps_dust->i_x = (unsigned)vlc_mrand48() % p_sys->i_width[Y_PLANE];
    ps_dust->i_y = (unsigned)vlc_mrand48() % p_sys->i_height[Y_PLANE];
    ps_dust->i_stop_trigger = p_sys->i_cur_time
                            + (uint64_t)vlc_mrand48() % HAIR_DURATION
                            + HAIR_DURATION / 2;
    ps_dust->i_x = MOD( (int32_t)vlc_mrand48(), p_sys->i_width[Y_PLANE]  );
    ps_dust->i_y = MOD( (int32_t)vlc_mrand48(), p_sys->i_height[Y_PLANE] );
    ps_dust->i_stop_trigger = p_sys->i_cur_time
                            + (uint64_t)vlc_mrand48() % DUST_DURATION
                            + DUST_DURATION / 2;
}
