 */
static void oldmovie_free_allocated_data( filter_t *p_filter ) {
    filter_sys_t *p_sys = p_filter->p_sys;
    for ( uint32_t i_s = 0; i_s < MAX_SCRATCH; i_s++ )
        FREENULL(p_sys->p_scratch[i_s]);
    for ( uint32_t i_h = 0; i_h < MAX_HAIR; i_h++ )
        FREENULL(p_sys->p_hair[i_h]);
    for ( uint32_t i_d = 0; i_d < MAX_DUST; i_d++ )
        FREENULL(p_sys->p_dust[i_d]);
    p_sys->i_planes = 0;
    FREENULL( p_sys->i_height );
    FREENULL( p_sys->i_width );
    FREENULL( p_sys->i_visible_pitch );
}
