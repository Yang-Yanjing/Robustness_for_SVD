 *****************************************************************************/
static int MouseEvent( filter_t *p_filter,
                       const vlc_mouse_t *p_old,
                       const vlc_mouse_t *p_new,
                       const video_format_t *p_fmt )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    VLC_UNUSED(p_old);
    if( !p_sys->b_vnc_mouse_events )
        return VLC_SUCCESS;
    int i_v = p_new->i_pressed;
    int i_x = p_new->i_x;
    int i_y = p_new->i_y;
    vlc_mutex_lock( &p_sys->lock );
    const int v_h = p_fmt->i_visible_height;
    const int v_w = p_sys->i_vnc_width * v_h / p_sys->i_vnc_height;
    const int v_x = (p_fmt->i_visible_width-v_w)/2;
    i_x -= v_x;
    if( i_y < 0 || i_x < 0 || i_y >= v_h || i_x >= v_w )
    {
        vlc_mutex_unlock( &p_sys->lock );
        msg_Dbg( p_filter, "invalid mouse event? x=%d y=%d btn=%x", i_x, i_y, i_v );
        return VLC_SUCCESS;
    }
    if( !p_sys->b_connection_active )
    {
        vlc_mutex_unlock( &p_sys->lock );
        return VLC_SUCCESS;
    }
#ifdef VNC_DEBUG
    msg_Dbg( p_filter, "mouse event x=%d y=%d btn=%x", i_x, i_y, i_v );
#endif
    /* */
    i_x = i_x * p_sys->i_vnc_width / v_w;
    i_y = i_y * p_sys->i_vnc_height / v_h;
    /* buttonMask bits 0-7 are buttons 1-8, 0=up, 1=down */
    rfbPointerEventMsg ev;
    ev.type = rfbPointerEvent;
    ev.buttonMask = i_v;
    ev.x = htons(i_x);
    ev.y = htons(i_y);
    write_exact( p_filter, p_sys->i_socket,
                 (char*)&ev, sz_rfbPointerEventMsg);
    vlc_mutex_unlock( &p_sys->lock );
    return VLC_EGENERIC;
}
