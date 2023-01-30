                          uint8_t **pp_data, int *pi_data );
static int HttpSetup( sout_stream_t *p_stream, const vlc_url_t *url)
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    p_sys->p_httpd_host = vlc_http_HostNew( VLC_OBJECT(p_stream) );
    if( p_sys->p_httpd_host )
    {
        p_sys->p_httpd_file = httpd_FileNew( p_sys->p_httpd_host,
                                             url->psz_path ? url->psz_path : "/",
                                             "application/sdp",
                                             NULL, NULL,
                                             HttpCallback, (void*)p_sys );
    }
    if( p_sys->p_httpd_file == NULL )
    {
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
