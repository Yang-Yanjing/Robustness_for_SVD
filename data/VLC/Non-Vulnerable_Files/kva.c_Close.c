 *****************************************************************************/
static void Close ( vlc_object_t *object )
{
    vout_display_t * vd = (vout_display_t *)object;
    vout_display_sys_t * sys = vd->sys;
    WinPostQueueMsg( sys->hmq, WM_QUIT, 0, 0 );
    DosWaitThread( &sys->tid, DCWW_WAIT );
    if( sys->pool )
        picture_pool_Delete( sys->pool );
    DosCloseEventSem( sys->ack_event );
    free( sys );
}
