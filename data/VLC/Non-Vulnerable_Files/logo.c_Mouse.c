}
static int Mouse( filter_t *p_filter, vlc_mouse_t *p_mouse,
                  const vlc_mouse_t *p_old, const vlc_mouse_t *p_new )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    vlc_mutex_lock( &p_sys->lock );
    logo_t *p_logo = LogoListCurrent( &p_sys->list );
    const picture_t *p_pic = p_logo->p_pic;
    if( p_pic )
    {
        const video_format_t *p_fmt = &p_pic->format;
        const int i_logo_w = p_fmt->i_visible_width;
        const int i_logo_h = p_fmt->i_visible_height;
        /* Check if we are over the logo */
        const bool b_over = p_new->i_x >= p_sys->i_pos_x &&
                            p_new->i_x <  p_sys->i_pos_x + i_logo_w &&
                            p_new->i_y >= p_sys->i_pos_y &&
                            p_new->i_y <  p_sys->i_pos_y + i_logo_h;
        if( b_over && vlc_mouse_HasPressed( p_old, p_new, MOUSE_BUTTON_LEFT ) )
            p_sys->b_mouse_grab = true;
        else if( vlc_mouse_HasReleased( p_old, p_new, MOUSE_BUTTON_LEFT ) )
            p_sys->b_mouse_grab = false;
        if( p_sys->b_mouse_grab )
        {
            int i_dx, i_dy;
            vlc_mouse_GetMotion( &i_dx, &i_dy, p_old, p_new );
            p_sys->i_pos_x = VLC_CLIP( p_sys->i_pos_x + i_dx, 0,
                                    p_filter->fmt_in.video.i_width  - i_logo_w );
            p_sys->i_pos_y = VLC_CLIP( p_sys->i_pos_y + i_dy, 0,
                                    p_filter->fmt_in.video.i_height - i_logo_h );
            /* object under mouse has moved */
            var_SetBool( p_filter->p_parent, "mouse-object", true );
        }
        else if( b_over )
        {
            /* object under mouse stoped moving */
            var_SetBool( p_filter->p_parent, "mouse-object", false );
        }
        if( p_sys->b_mouse_grab || b_over )
        {
            vlc_mutex_unlock( &p_sys->lock );
            return VLC_EGENERIC;
        }
    }
    vlc_mutex_unlock( &p_sys->lock );
    *p_mouse = *p_new;
    return VLC_SUCCESS;
}
