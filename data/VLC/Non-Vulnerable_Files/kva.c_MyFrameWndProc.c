}
static MRESULT EXPENTRY MyFrameWndProc( HWND hwnd, ULONG msg, MPARAM mp1,
                                        MPARAM mp2 )
{
    vout_display_t *vd = WinQueryWindowPtr( hwnd, 0 );
    vout_display_sys_t *sys = vd->sys;
    switch( msg )
    {
        case WM_QUERYTRACKINFO :
        {
            MRESULT mr;
            mr = sys->p_old_frame( hwnd, msg, mp1, mp2 );
            if( !sys->b_fixt23 )
                return mr;
            PTRACKINFO pti = ( PTRACKINFO )mp2;
            RECTL      rcl;
            pti->rclBoundary.xLeft   = 0;
            pti->rclBoundary.yBottom = 0;
            pti->rclBoundary.xRight  = sys->i_screen_width;
            pti->rclBoundary.yTop    = sys->i_screen_height;
            WinCalcFrameRect( hwnd, &pti->rclBoundary, FALSE );
            pti->ptlMaxTrackSize.x = pti->rclBoundary.xRight -
                                     pti->rclBoundary.xLeft;
            pti->ptlMaxTrackSize.y = pti->rclBoundary.yTop -
                                     pti->rclBoundary.yBottom;
            rcl.xLeft   = 0;
            rcl.yBottom = 0;
            rcl.xRight  = sys->kvas.szlSrcSize.cx + 1;
            rcl.yTop    = sys->kvas.szlSrcSize.cy + 1;
            WinCalcFrameRect( hwnd, &rcl, FALSE );
            pti->ptlMinTrackSize.x = rcl.xRight - rcl.xLeft;
            pti->ptlMinTrackSize.y = rcl.yTop - rcl.yBottom;
            return MRFROMLONG( TRUE );
        }
        case WM_ADJUSTWINDOWPOS :
        {
            if( !sys->b_fixt23 )
                break;
            PSWP  pswp = ( PSWP )mp1;
            if( pswp->fl & SWP_SIZE )
            {
                RECTL rcl;
                rcl.xLeft   = pswp->x;
                rcl.yBottom = pswp->y;
                rcl.xRight  = rcl.xLeft + pswp->cx;
                rcl.yTop    = rcl.yBottom + pswp->cy;
                WinCalcFrameRect( hwnd, &rcl, TRUE );
                if( rcl.xRight - rcl.xLeft <= sys->kvas.szlSrcSize.cx )
                    rcl.xRight = rcl.xLeft + ( sys->kvas.szlSrcSize.cx + 1 );
                if( rcl.yTop - rcl.yBottom <= sys->kvas.szlSrcSize.cy )
                    rcl.yTop = rcl.yBottom + ( sys->kvas.szlSrcSize.cy + 1 );
                if( rcl.xRight - rcl.xLeft > sys->i_screen_width )
                {
                    rcl.xLeft  = 0;
                    rcl.xRight = sys->i_screen_width;
                }
                if( rcl.yTop - rcl.yBottom > sys->i_screen_height )
                {
                    rcl.yBottom = 0;
                    rcl.yTop    = sys->i_screen_height;
                }
                WinCalcFrameRect( hwnd, &rcl, FALSE );
                if( pswp->x != rcl.xLeft || pswp->y != rcl.yBottom )
                    pswp->fl |= SWP_MOVE;
                pswp->x = rcl.xLeft;
                pswp->y = rcl.yBottom;
                pswp->cx = rcl.xRight - rcl.xLeft;
                pswp->cy = rcl.yTop - rcl.yBottom;
            }
            break;
        }
        //case WM_VRNDISABLED :
        case WM_VRNENABLED :
            if( !vd->cfg->is_fullscreen && sys->is_on_top )
                WinSetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER );
            break;
    }
    return sys->p_old_frame( hwnd, msg, mp1, mp2 );
}
