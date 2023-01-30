 */
static int freeze_allocate_data( filter_t *p_filter, picture_t *p_pic_in )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    freeze_free_allocated_data( p_filter );
   /*
    * take into account different characteristics for each plane
    */
    p_sys->i_planes        = p_pic_in->i_planes;
    p_sys->i_height        = calloc( p_sys->i_planes, sizeof(int32_t) );
    p_sys->i_width         = calloc( p_sys->i_planes, sizeof(int32_t) );
    p_sys->i_visible_pitch = calloc( p_sys->i_planes, sizeof(int32_t) );
    if ( unlikely( !p_sys->i_height || !p_sys->i_width || !p_sys->i_visible_pitch ) )
    {
        freeze_free_allocated_data( p_filter );
        return VLC_ENOMEM;
    }
    /* init data */
    for ( int32_t i_p = 0; i_p < p_sys->i_planes; i_p++ )
    {
        p_sys->i_visible_pitch [i_p] = (int) p_pic_in->p[i_p].i_visible_pitch;
        p_sys->i_height[i_p]         = (int) p_pic_in->p[i_p].i_visible_lines;
        p_sys->i_width[i_p]          = (int) p_pic_in->p[i_p].i_visible_pitch
                                     / p_pic_in->p[i_p].i_pixel_pitch;
    }
    /* buffer used to countdown freezing delay */
    p_sys->pi_freezing_countdown
        = calloc( p_sys->i_height[Y_PLANE], sizeof(int16_t*) );
    if ( unlikely( !p_sys->pi_freezing_countdown ) )
    {
        freeze_free_allocated_data( p_filter );
        return VLC_ENOMEM;
    }
    for ( int32_t i_r = 0; i_r < p_sys->i_height[Y_PLANE]; i_r++ )
    {
        p_sys->pi_freezing_countdown[i_r]
            = calloc( p_sys->i_width[Y_PLANE], sizeof(int16_t) );
        if ( unlikely( !p_sys->pi_freezing_countdown[i_r] ) )
        {
            freeze_free_allocated_data( p_filter );
            return VLC_ENOMEM;
        }
    }
    /* buffer used to cache freezed pixels colors */
    p_sys->pi_freezed_picture = calloc( p_sys->i_planes, sizeof(int8_t**) );
    if( unlikely( !p_sys->pi_freezed_picture ) )
    {
        freeze_free_allocated_data( p_filter );
        return VLC_ENOMEM;
    }
    for ( int32_t i_p = 0; i_p < p_sys->i_planes; i_p++)
    {
        p_sys->pi_freezed_picture[i_p]
            = calloc( p_sys->i_height[i_p], sizeof(int8_t*) );
        if ( unlikely(!p_sys->pi_freezed_picture[i_p]) )
        {
            freeze_free_allocated_data( p_filter );
            return VLC_ENOMEM;
        }
        for ( int32_t i_r = 0; i_r < p_sys->i_height[i_p]; i_r++ )
        {
            p_sys->pi_freezed_picture[i_p][i_r]
                = calloc( p_sys->i_width[i_p], sizeof(int8_t) );
            if ( unlikely( !p_sys->pi_freezed_picture[i_p][i_r] ) )
            {
                freeze_free_allocated_data( p_filter );
                return VLC_ENOMEM;
            }
        }
    }
    /* flag used to manage freezed pixels cache update */
    p_sys->pb_update_cache
        = calloc( p_sys->i_height[Y_PLANE], sizeof(bool*) );
    if( unlikely( !p_sys->pb_update_cache ) )
    {
        freeze_free_allocated_data( p_filter );
        return VLC_ENOMEM;
    }
    for ( int32_t i_r = 0; i_r < p_sys->i_height[Y_PLANE]; i_r++ )
    {
        p_sys->pb_update_cache[i_r]
            = calloc( p_sys->i_width[Y_PLANE], sizeof(bool) );
        if ( unlikely( !p_sys->pb_update_cache[i_r] ) )
        {
            freeze_free_allocated_data( p_filter );
            return VLC_ENOMEM;
        }
    }
    return VLC_SUCCESS;
}
