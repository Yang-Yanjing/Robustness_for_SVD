}
static int MovedEvent( vlc_object_t *p_this, char const *psz_var,
                       vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    intf_thread_t *p_intf = (intf_thread_t *)p_data;
    intf_sys_t    *p_sys = p_intf->p_sys;
    (void) p_this; (void) psz_var; (void) oldval;
    vlc_mutex_lock( &p_sys->lock );
    if( p_sys->b_button_pressed )
    {
        int i_horizontal = newval.coords.x - p_sys->i_last_x;
        int i_vertical = newval.coords.y - p_sys->i_last_y;
        unsigned int pattern = 0;
        i_horizontal = i_horizontal / p_sys->i_threshold;
        i_vertical = i_vertical / p_sys->i_threshold;
        if( i_horizontal < 0 )
        {
            msg_Dbg( p_intf, "left gesture (%d)", i_horizontal );
            pattern = LEFT;
        }
        else if( i_horizontal > 0 )
        {
            msg_Dbg( p_intf, "right gesture (%d)", i_horizontal );
            pattern = RIGHT;
        }
        if( i_vertical < 0 )
        {
            msg_Dbg( p_intf, "up gesture (%d)", i_vertical );
            pattern = UP;
        }
        else if( i_vertical > 0 )
        {
            msg_Dbg( p_intf, "down gesture (%d)", i_vertical );
            pattern = DOWN;
        }
        if( pattern )
        {
            p_sys->i_last_x = newval.coords.x;
            p_sys->i_last_y = newval.coords.y;
            if( p_sys->i_num_gestures > 0
             && gesture( p_sys->i_pattern, p_sys->i_num_gestures - 1 )
                    != pattern )
            {
                p_sys->i_pattern |= pattern << ( p_sys->i_num_gestures * 4 );
                p_sys->i_num_gestures++;
            }
            else if( p_sys->i_num_gestures == 0 )
            {
                p_sys->i_pattern = pattern;
                p_sys->i_num_gestures++;
            }
        }
    }
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
