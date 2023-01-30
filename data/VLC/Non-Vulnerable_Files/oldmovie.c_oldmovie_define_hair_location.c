 */
static void oldmovie_define_hair_location( filter_t *p_filter, hair_t* ps_hair ) {
    filter_sys_t *p_sys = p_filter->p_sys;
    ps_hair->i_x = (unsigned)vlc_mrand48() % p_sys->i_width[Y_PLANE];
    ps_hair->i_y = (unsigned)vlc_mrand48() % p_sys->i_height[Y_PLANE];
    ps_hair->i_rotation = (unsigned)vlc_mrand48() % 200;
    ps_hair->i_stop_trigger = p_sys->i_cur_time
                            + (uint64_t)vlc_mrand48() % HAIR_DURATION
                            + HAIR_DURATION / 2;
}
