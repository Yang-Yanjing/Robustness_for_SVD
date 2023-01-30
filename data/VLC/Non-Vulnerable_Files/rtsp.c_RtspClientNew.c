static
rtsp_session_t *RtspClientNew( rtsp_stream_t *rtsp )
{
    rtsp_session_t *s = malloc( sizeof( *s ) );
    if( s == NULL )
        return NULL;
    s->stream = rtsp;
    vlc_rand_bytes (&s->id, sizeof (s->id));
    s->trackc = 0;
    s->trackv = NULL;
    TAB_APPEND( rtsp->sessionc, rtsp->sessionv, s );
    return s;
}
