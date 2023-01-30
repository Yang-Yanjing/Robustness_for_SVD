}
static void MouseReleased( vout_display_t *vd, unsigned button )
{
    vd->sys->button_pressed &= ~(1 << button);
    if( !vd->sys->button_pressed )
        WinSetCapture( HWND_DESKTOP, NULLHANDLE );
    vout_display_SendEventMouseReleased( vd, button );
}
