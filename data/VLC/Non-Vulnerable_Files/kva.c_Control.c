 *****************************************************************************/
static int Control( vout_display_t *vd, int query, va_list args )
{
    vout_display_sys_t *sys = vd->sys;
    switch (query)
    {
    case VOUT_DISPLAY_HIDE_MOUSE:
    {
        POINTL ptl;
        WinQueryPointerPos( HWND_DESKTOP, &ptl );
        if( !sys->is_mouse_hidden &&
            WinWindowFromPoint( HWND_DESKTOP, &ptl, TRUE ) == sys->client )
        {
            WinShowPointer( HWND_DESKTOP, FALSE );
            sys->is_mouse_hidden = true;
        }
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_CHANGE_FULLSCREEN:
    {
        vout_display_cfg_t cfg = *va_arg(args, const vout_display_cfg_t *);
        if( sys->parent_window )
            vout_window_SetFullScreen(sys->parent_window, cfg.is_fullscreen);
        else
            WinPostMsg( sys->client, WM_VLC_FULLSCREEN_CHANGE,
                        MPFROMLONG( cfg.is_fullscreen ), 0 );
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_CHANGE_WINDOW_STATE:
    {
        const unsigned state = va_arg( args, unsigned );
        const bool is_on_top = (state & VOUT_WINDOW_STATE_ABOVE) != 0;
        if( sys->parent_window )
        {
            if( vout_window_SetState( sys->parent_window, state ))
                return VLC_EGENERIC;
        }
        else if( is_on_top )
            WinSetWindowPos( sys->frame, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER );
        sys->is_on_top = is_on_top;
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_CHANGE_DISPLAY_SIZE:
    case VOUT_DISPLAY_CHANGE_ZOOM:
    {
        const vout_display_cfg_t *cfg = va_arg(args, const vout_display_cfg_t *);
        bool  is_forced = query == VOUT_DISPLAY_CHANGE_ZOOM ||
                          va_arg(args, int);
        if( is_forced )
        {
            if( sys->parent_window )
            {
                vout_window_SetSize(sys->parent_window,
                                    cfg->display.width, cfg->display.height);
            }
            else
                WinPostMsg( sys->client, WM_VLC_SIZE_CHANGE,
                            MPFROMLONG( cfg->display.width ),
                            MPFROMLONG( cfg->display.height ));
        }
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_CHANGE_SOURCE_ASPECT:
    case VOUT_DISPLAY_CHANGE_SOURCE_CROP:
    {
        const video_format_t *src = va_arg(args, const video_format_t *);
        if( query == VOUT_DISPLAY_CHANGE_SOURCE_ASPECT )
        {
            vout_display_place_t place;
            vout_display_PlacePicture(&place, src, vd->cfg, false);
            sys->kvas.ulAspectWidth  = place.width;
            sys->kvas.ulAspectHeight = place.height;
        }
        else
        {
            video_format_t src_rot;
            video_format_ApplyRotation(&src_rot, src);
            sys->kvas.rclSrcRect.xLeft   = src_rot.i_x_offset;
            sys->kvas.rclSrcRect.yTop    = src_rot.i_y_offset;
            sys->kvas.rclSrcRect.xRight  = src_rot.i_x_offset +
                                           src_rot.i_visible_width;
            sys->kvas.rclSrcRect.yBottom = src_rot.i_y_offset +
                                           src_rot.i_visible_height;
        }
        kvaSetup( &sys->kvas );
        return VLC_SUCCESS;
    }
    case VOUT_DISPLAY_RESET_PICTURES:
    case VOUT_DISPLAY_CHANGE_DISPLAY_FILLED:
    case VOUT_DISPLAY_GET_OPENGL:
        /* TODO */
        break;
    }
    msg_Err(vd, "Unsupported query(=%d) in vout display KVA", query);
    return VLC_EGENERIC;
}
