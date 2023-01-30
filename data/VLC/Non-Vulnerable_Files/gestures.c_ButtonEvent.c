}
static int ButtonEvent( vlc_object_t *p_this, char const *psz_var,
                        vlc_value_t oldval, vlc_value_t val, void *p_data )
{
    intf_thread_t *p_intf = p_data;
    intf_sys_t *p_sys = p_intf->p_sys;
    (void) psz_var; (void) oldval;
    vlc_mutex_lock( &p_sys->lock );
    if( val.i_int & p_sys->i_button_mask )
    {
        if( !p_sys->b_button_pressed )
        {
            p_sys->b_button_pressed = true;
            var_GetCoords( p_this, "mouse-moved",
                           &p_sys->i_last_x, &p_sys->i_last_y );
        }
    }
    else
    {
        if( p_sys->b_button_pressed )
        {
            p_sys->b_button_pressed = false;
            ProcessGesture( p_intf );
        }
    }
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
