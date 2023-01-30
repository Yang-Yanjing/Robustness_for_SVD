}
static void MousePressed( vout_display_t *vd, HWND hwnd, unsigned button )
{
    if( WinQueryFocus( HWND_DESKTOP ) != hwnd )
        WinSetFocus( HWND_DESKTOP, hwnd );
    if( !vd->sys->button_pressed )
        WinSetCapture( HWND_DESKTOP, hwnd );
    vd->sys->button_pressed |= 1 << button;
    vout_display_SendEventMousePressed( vd, button );
}
