 */
static int Open ( vlc_object_t *object )
{
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;
    vd->sys = sys = calloc( 1, sizeof( *sys ));
    if( !sys )
        return VLC_ENOMEM;
    DosCreateEventSem( NULL, &sys->ack_event, 0, FALSE );
    sys->tid = _beginthread( PMThread, NULL, 1024 * 1024, vd );
    DosWaitEventSem( sys->ack_event, SEM_INDEFINITE_WAIT );
    if( sys->i_result != VLC_SUCCESS )
    {
        DosCloseEventSem( sys->ack_event );
        free( sys );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
