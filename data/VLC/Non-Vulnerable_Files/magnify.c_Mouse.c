}
static int Mouse( filter_t *p_filter, vlc_mouse_t *p_mouse, const vlc_mouse_t *p_old, const vlc_mouse_t *p_new )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    const video_format_t *p_fmt = &p_filter->fmt_in.video;
    /* */
    const bool b_click = vlc_mouse_HasPressed( p_old, p_new, MOUSE_BUTTON_LEFT );
    const bool b_pressed = vlc_mouse_IsLeftPressed( p_new );
    bool b_grab = false;
    /* Find the mouse position */
    if( p_sys->b_visible )
    {
        const int i_visu_width  = p_fmt->i_width  / VIS_ZOOM;
        const int i_visu_height = p_fmt->i_height / VIS_ZOOM;
        if( p_new->i_x >= 0 && p_new->i_x < i_visu_width &&
            p_new->i_y >= 0 && p_new->i_y < i_visu_height )
        {
            /* Visualization */
            if( b_pressed )
            {
                const int v_w = p_fmt->i_width  * ZOOM_FACTOR / p_sys->i_zoom;
                const int v_h = p_fmt->i_height * ZOOM_FACTOR / p_sys->i_zoom;
                p_sys->i_x = VLC_CLIP( p_new->i_x * VIS_ZOOM - v_w/2, 0,
                                           (int)p_fmt->i_width  - v_w - 1);
                p_sys->i_y = VLC_CLIP( p_new->i_y * VIS_ZOOM - v_h/2, 0,
                                           (int)p_fmt->i_height - v_h - 1);
                b_grab = true;
            }
        }
        else if( p_new->i_x >= 0 && p_new->i_x < 80 &&
                 p_new->i_y >= i_visu_height &&
                 p_new->i_y <  i_visu_height + 9 )
        {
            /* Hide text */
            if( b_click )
            {
                p_sys->b_visible = false;
                b_grab = true;
            }
        }
        else if( p_new->i_x >= 0 &&
                 p_new->i_x <= ( i_visu_height + 90 - p_new->i_y ) *
                               ( i_visu_height + 90 - p_new->i_y ) / 160 &&
                 p_new->i_y >= i_visu_height + 9 &&
                 p_new->i_y <= i_visu_height + 90 )
        {
            /* Zoom gauge */
            if( b_pressed )
            {
                p_sys->i_zoom = __MAX( ZOOM_FACTOR,
                                       (80 + i_visu_height - p_new->i_y + 2) *
                                           ZOOM_FACTOR / 10 );
                const int v_w = p_fmt->i_width  * ZOOM_FACTOR / p_sys->i_zoom;
                const int v_h = p_fmt->i_height * ZOOM_FACTOR / p_sys->i_zoom;
                p_sys->i_x = VLC_CLIP( p_sys->i_x, 0, (int)p_fmt->i_width  - v_w - 1 );
                p_sys->i_y = VLC_CLIP( p_sys->i_y, 0, (int)p_fmt->i_height - v_h - 1 );
                b_grab = true;
            }
        }
    }
    else
    {
        if( p_new->i_x >= 0 && p_new->i_x <  80 &&
            p_new->i_y >= 0 && p_new->i_y <= 10 )
        {
            /* Show text */
            if( b_click )
            {
                p_sys->b_visible = true;
                b_grab = true;
            }
        }
    }
    if( vlc_mouse_HasMoved( p_old, p_new ) )
        p_sys->i_last_activity = mdate();
    if( b_grab )
        return VLC_EGENERIC;
    /* */
    *p_mouse = *p_new;
    p_mouse->i_x = p_sys->i_x + p_new->i_x * ZOOM_FACTOR / p_sys->i_zoom;
    p_mouse->i_y = p_sys->i_y + p_new->i_y * ZOOM_FACTOR / p_sys->i_zoom;
    return VLC_SUCCESS;
}
