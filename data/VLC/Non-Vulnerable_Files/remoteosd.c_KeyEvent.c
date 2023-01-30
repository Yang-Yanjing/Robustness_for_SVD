 *****************************************************************************/
static int KeyEvent( vlc_object_t *p_this, char const *psz_var,
                     vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(psz_var); VLC_UNUSED(oldval);
    filter_t *p_filter = (filter_t *)p_data;
    filter_sys_t *p_sys = p_filter->p_sys;
    if( !p_sys->b_vnc_key_events )
        return VLC_SUCCESS;
    msg_Dbg( p_this, "key pressed (%"PRId64") ", newval.i_int );
    if ( !newval.i_int )
    {
        msg_Err( p_this, "Received invalid key event 0" );
        return VLC_EGENERIC;
    }
    vlc_mutex_lock( &p_sys->lock );
    if( !p_sys->b_connection_active )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return VLC_SUCCESS;
    }
    uint32_t i_key32 = newval.i_int;
    i_key32 = htonl(i_key32);
    rfbKeyEventMsg ev;
    ev.type = rfbKeyEvent;
    ev.down = 1;
    ev.pad = 0;
    /* first key-down for modifier-keys */
    if (newval.i_int & KEY_MODIFIER_CTRL)
    {
        ev.key = 0xffe3;
        write_exact( p_filter, p_sys->i_socket,
                     (char*)&ev, sz_rfbKeyEventMsg);
    }
    if (newval.i_int & KEY_MODIFIER_SHIFT)
    {
        ev.key = 0xffe1;
        write_exact( p_filter, p_sys->i_socket,
                     (char*)&ev, sz_rfbKeyEventMsg);
    }
    if (newval.i_int & KEY_MODIFIER_ALT)
    {
        ev.key = 0xffe9;
        write_exact( p_filter, p_sys->i_socket,
                     (char*)&ev, sz_rfbKeyEventMsg);
    }
    /* then key-down for the pressed key */
    ev.key = i_key32;
    write_exact( p_filter, p_sys->i_socket,
                 (char*)&ev, sz_rfbKeyEventMsg);
    ev.down = 0;
    /* then key-up for the pressed key */
    write_exact( p_filter, p_sys->i_socket,
                 (char*)&ev, sz_rfbKeyEventMsg);
    /* last key-down for modifier-keys */
    if (newval.i_int & KEY_MODIFIER_CTRL)
    {
        ev.key = 0xffe3;
        write_exact( p_filter, p_sys->i_socket,
                     (char*)&ev, sz_rfbKeyEventMsg);
    }
    if (newval.i_int & KEY_MODIFIER_SHIFT)
    {
        ev.key = 0xffe1;
        write_exact( p_filter, p_sys->i_socket,
                     (char*)&ev, sz_rfbKeyEventMsg);
    }
    if (newval.i_int & KEY_MODIFIER_ALT)
    {
       ev.key = 0xffe9;
       write_exact( p_filter, p_sys->i_socket,
                    (char*)&ev, sz_rfbKeyEventMsg);
    }
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_SUCCESS;
}
