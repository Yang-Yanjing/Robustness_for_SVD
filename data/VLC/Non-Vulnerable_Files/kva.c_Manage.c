 *****************************************************************************/
static void Manage( vout_display_t *vd )
{
    vout_display_sys_t * sys = vd->sys;
    /* Let a window procedure manage instead because if resizing a frame window
     * here, WM_SIZE is not sent to its child window.
     * Maybe, is this due to the different threads ? */
    WinPostMsg( sys->client, WM_VLC_MANAGE, 0, 0 );
}
