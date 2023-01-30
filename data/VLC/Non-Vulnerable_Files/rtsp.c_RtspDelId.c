}
void RtspDelId( rtsp_stream_t *rtsp, rtsp_stream_id_t *id )
{
    httpd_UrlDelete( id->url );
    vlc_mutex_lock( &rtsp->lock );
    for( int i = 0; i < rtsp->sessionc; i++ )
    {
        rtsp_session_t *ses = rtsp->sessionv[i];
        for( int j = 0; j < ses->trackc; j++ )
        {
            if( ses->trackv[j].id == id )
            {
                rtsp_strack_t *tr = ses->trackv + j;
                RtspTrackClose( tr );
                REMOVE_ELEM( ses->trackv, ses->trackc, j );
            }
        }
    }
    vlc_mutex_unlock( &rtsp->lock );
    free( id );
}
