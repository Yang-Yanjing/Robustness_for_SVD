 **/
static int freeze_mouse( filter_t *p_filter, vlc_mouse_t *p_mouse,
                  const vlc_mouse_t *p_old, const vlc_mouse_t *p_new )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    const video_format_t  *p_fmt_in = &p_filter->fmt_in.video;
    /* Only take events inside the video area */
    if( p_new->i_x < 0 || p_new->i_x >= (int)p_fmt_in->i_width ||
        p_new->i_y < 0 || p_new->i_y >= (int)p_fmt_in->i_height )
        return VLC_EGENERIC;
    if ( unlikely(!p_sys->b_init) )
    {
        *p_mouse = *p_new;
        return VLC_SUCCESS;
    }
    int32_t i_base_timeout = 0;
    if( vlc_mouse_HasPressed( p_old, p_new, MOUSE_BUTTON_LEFT ) )
        i_base_timeout = 100;
    else if( vlc_mouse_IsLeftPressed( p_new ) )
        i_base_timeout = 50;
    if( i_base_timeout > 0 )
    {
        /*
         * find pixels selected by user to apply freezing filter
         */
        int32_t i_min_sq_radius = (p_sys->i_width[Y_PLANE] / 15)
                                * (p_sys->i_width[Y_PLANE] / 15);
        for ( int32_t i_r = 0; i_r < p_sys->i_height[Y_PLANE]; i_r++)
            for ( int32_t i_c = 0; i_c < p_sys->i_width[Y_PLANE]; i_c++)
            {
                int32_t i_sq_dist =   ( p_new->i_x - i_c )
                                    * ( p_new->i_x - i_c )
                                    + ( p_new->i_y - i_r )
                                    * ( p_new->i_y - i_r );
                i_sq_dist = __MAX(0, i_sq_dist - i_min_sq_radius);
                uint16_t i_timeout = __MAX(i_base_timeout - i_sq_dist, 0);
                /* ask to update chache for pixel to be freezed just now */
                if ( p_sys->pi_freezing_countdown[i_r][i_c] == 0 && i_timeout > 0)
                     p_sys->pb_update_cache[i_r][i_c] = true;
                /* set freezing delay */
                if ( p_sys->pi_freezing_countdown[i_r][i_c] < i_timeout )
                     p_sys->pi_freezing_countdown[i_r][i_c] = i_timeout;
            }
    }
    return VLC_EGENERIC;
}
