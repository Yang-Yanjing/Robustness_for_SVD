#define WM_MOUSELEAVE   0x41F
static MRESULT EXPENTRY WndProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    vout_display_t * vd = WinQueryWindowPtr( hwnd, 0 );
    MRESULT result = ( MRESULT )TRUE;
    if ( !vd )
        return WinDefWindowProc( hwnd, msg, mp1, mp2 );
    vout_display_sys_t * sys = vd->sys;
    RECTL rcl;
    SWP   swp;
    if ( sys->is_mouse_hidden &&
         ((msg >= WM_MOUSEFIRST    && msg <= WM_MOUSELAST) ||
          (msg >= WM_EXTMOUSEFIRST && msg <= WM_EXTMOUSELAST) ||
           msg == WM_MOUSELEAVE))
    {
        WinShowPointer(HWND_DESKTOP, TRUE);
        sys->is_mouse_hidden = false;
    }
    switch( msg )
    {
        /* the user wants to close the window */
        case WM_CLOSE:
            vout_display_SendEventClose(vd);
            result = 0;
            break;
        case WM_MOUSEMOVE :
        {
            SHORT i_mouse_x = SHORT1FROMMP( mp1 );
            SHORT i_mouse_y = SHORT2FROMMP( mp1 );
            RECTL movie_rect;
            int   i_movie_width, i_movie_height;
            int   i_src_width, i_src_height;
            /* Get a current movie area */
            kvaAdjustDstRect( &sys->kvas.rclSrcRect, &movie_rect );
            i_movie_width = movie_rect.xRight - movie_rect.xLeft;
            i_movie_height = movie_rect.yTop - movie_rect.yBottom;
            i_src_width =  sys->kvas.rclSrcRect.xRight -
                           sys->kvas.rclSrcRect.xLeft;
            i_src_height = sys->kvas.rclSrcRect.yBottom -
                           sys->kvas.rclSrcRect.yTop;
            int x = ( i_mouse_x - movie_rect.xLeft ) *
                    i_src_width / i_movie_width +
                    sys->kvas.rclSrcRect.xLeft;
            int y = ( i_mouse_y - movie_rect.yBottom ) *
                    i_src_height / i_movie_height;
            /* Invert Y coordinate and add y offset */
            y = ( i_src_height - y ) + sys->kvas.rclSrcRect.yTop;;
            vout_display_SendEventMouseMoved(vd, x, y);
            result = WinDefWindowProc( hwnd, msg, mp1,mp2 );
            break;
        }
        case WM_BUTTON1DOWN :
            MousePressed( vd, hwnd, MOUSE_BUTTON_LEFT );
            break;
        case WM_BUTTON2DOWN :
            MousePressed( vd, hwnd, MOUSE_BUTTON_RIGHT );
            break;
        case WM_BUTTON3DOWN :
            MousePressed( vd, hwnd, MOUSE_BUTTON_CENTER );
            break;
        case WM_BUTTON1UP :
            MouseReleased( vd, MOUSE_BUTTON_LEFT );
            break;
        case WM_BUTTON2UP :
            MouseReleased( vd, MOUSE_BUTTON_RIGHT );
            break;
        case WM_BUTTON3UP :
            MouseReleased( vd, MOUSE_BUTTON_CENTER );
            break;
        case WM_BUTTON1DBLCLK :
            vout_display_SendEventMouseDoubleClick(vd);
            break;
        case WM_TRANSLATEACCEL :
            /* We have no accelerator table at all */
            result = ( MRESULT )FALSE;
            break;
        case WM_CHAR :
        {
            USHORT i_flags = SHORT1FROMMP( mp1 );
            USHORT i_ch    = SHORT1FROMMP( mp2 );
            USHORT i_vk    = SHORT2FROMMP( mp2 );
            int    i_key   = 0;
            /* If embedded window, let the parent process keys */
            if( sys->parent_window )
            {
                WinPostMsg( sys->parent, msg, mp1, mp2 );
                break;
            }
            if( !( i_flags & KC_KEYUP ))
            {
                if( i_flags & KC_VIRTUALKEY )
                    /* convert the key if possible */
                    i_key = ConvertKey( i_vk );
                else if(( i_flags & KC_CHAR ) && !HIBYTE( i_ch ))
                    i_key = tolower( i_ch );
                if( i_key )
                {
                    if( i_flags & KC_SHIFT )
                       i_key |= KEY_MODIFIER_SHIFT;
                    if( i_flags & KC_CTRL )
                        i_key |= KEY_MODIFIER_CTRL;
                    if( i_flags & KC_ALT )
                        i_key |= KEY_MODIFIER_ALT;
                    vout_display_SendEventKey(vd, i_key);
                }
            }
            break;
        }
        /* Process Manage() call */
        case WM_VLC_MANAGE :
            break;
        /* Fullscreen change */
        case WM_VLC_FULLSCREEN_CHANGE :
            if( LONGFROMMP( mp1 ))
            {
                WinQueryWindowPos( sys->frame, &swp );
                sys->client_rect.xLeft   = swp.x;
                sys->client_rect.yBottom = swp.y;
                sys->client_rect.xRight  = sys->client_rect.xLeft   + swp.cx;
                sys->client_rect.yTop    = sys->client_rect.yBottom + swp.cy;
                WinCalcFrameRect( sys->frame, &sys->client_rect, TRUE );
                rcl.xLeft   = 0;
                rcl.yBottom = 0;
                rcl.xRight  = sys->i_screen_width;
                rcl.yTop    = sys->i_screen_height;
            }
            else
                rcl = sys->client_rect;
            WinCalcFrameRect( sys->frame, &rcl, FALSE );
            WinSetWindowPos( sys->frame, HWND_TOP,
                             rcl.xLeft, rcl.yBottom,
                             rcl.xRight - rcl.xLeft, rcl.yTop - rcl.yBottom,
                             SWP_MOVE | SWP_SIZE | SWP_ZORDER | SWP_SHOW |
                             SWP_ACTIVATE );
            break;
        /* Size change */
        case WM_VLC_SIZE_CHANGE :
            rcl.xLeft   = 0;
            rcl.yBottom = 0;
            rcl.xRight  = LONGFROMMP( mp1 );
            rcl.yTop    = LONGFROMMP( mp2 );
            WinCalcFrameRect( sys->frame, &rcl, FALSE );
            WinSetWindowPos( sys->frame, NULLHANDLE,
                             0, 0,
                             rcl.xRight - rcl.xLeft, rcl.yTop - rcl.yBottom,
                             SWP_SIZE );
            WinQueryWindowPos( sys->frame, &swp );
            sys->client_rect.xLeft   = swp.x;
            sys->client_rect.yBottom = swp.y;
            sys->client_rect.xRight  = sys->client_rect.xLeft   + swp.cx;
            sys->client_rect.yTop    = sys->client_rect.yBottom + swp.cy;
            WinCalcFrameRect( sys->frame, &sys->client_rect, TRUE );
            break;
        default :
            return WinDefWindowProc( hwnd, msg, mp1, mp2 );
    }
    /* If embedded window, we need to change our window size according to a
     * parent window size */
    if( sys->parent_window )
    {
        WinQueryWindowRect( sys->parent, &rcl );
        if( rcl.xLeft   != sys->parent_rect.xLeft   ||
            rcl.yBottom != sys->parent_rect.yBottom ||
            rcl.xRight  != sys->parent_rect.xRight  ||
            rcl.yTop    != sys->parent_rect.yTop)
        {
            sys->parent_rect = rcl;
            WinCalcFrameRect( sys->frame, &rcl, FALSE );
            WinSetWindowPos( sys->frame, NULLHANDLE,
                             rcl.xLeft, rcl.yBottom,
                             rcl.xRight - rcl.xLeft, rcl.yTop - rcl.yBottom,
                             SWP_SIZE | SWP_MOVE );
        }
    }
    return result;
}
