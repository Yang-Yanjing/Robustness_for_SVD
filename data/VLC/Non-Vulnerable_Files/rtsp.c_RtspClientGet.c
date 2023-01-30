static
rtsp_session_t *RtspClientGet( rtsp_stream_t *rtsp, const char *name )
{
    char *end;
    uint64_t id;
    int i;
    if( name == NULL )
        return NULL;
    errno = 0;
    id = strtoull( name, &end, 0x10 );
    if( errno || *end )
        return NULL;
    /* FIXME: use a hash/dictionary */
    for( i = 0; i < rtsp->sessionc; i++ )
    {
        if( rtsp->sessionv[i]->id == id )
            return rtsp->sessionv[i];
    }
    return NULL;
}
