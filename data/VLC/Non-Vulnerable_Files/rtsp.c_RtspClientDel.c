static
void RtspClientDel( rtsp_stream_t *rtsp, rtsp_session_t *session )
{
    int i;
    TAB_REMOVE( rtsp->sessionc, rtsp->sessionv, session );
    for( i = 0; i < session->trackc; i++ )
        RtspTrackClose( &session->trackv[i] );
    free( session->trackv );
    free( session );
}
