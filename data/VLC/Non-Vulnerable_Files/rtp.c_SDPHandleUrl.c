 *****************************************************************************/
static void SDPHandleUrl( sout_stream_t *p_stream, const char *psz_url )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    vlc_url_t url;
    vlc_UrlParse( &url, psz_url, 0 );
    if( url.psz_protocol && !strcasecmp( url.psz_protocol, "http" ) )
    {
        if( p_sys->p_httpd_file )
        {
            msg_Err( p_stream, "you can use sdp=http:// only once" );
            goto out;
        }
        if( HttpSetup( p_stream, &url ) )
        {
            msg_Err( p_stream, "cannot export SDP as HTTP" );
        }
    }
    else if( url.psz_protocol && !strcasecmp( url.psz_protocol, "rtsp" ) )
    {
        if( p_sys->rtsp != NULL )
        {
            msg_Err( p_stream, "you can use sdp=rtsp:// only once" );
            goto out;
        }
        if( url.psz_host != NULL && *url.psz_host )
        {
            msg_Warn( p_stream, "\"%s\" RTSP host might be ignored in "
                      "multiple-host configurations, use at your own risks.",
                      url.psz_host );
            msg_Info( p_stream, "Consider passing --rtsp-host=IP on the "
                                "command line instead." );
            var_Create( p_stream, "rtsp-host", VLC_VAR_STRING );
            var_SetString( p_stream, "rtsp-host", url.psz_host );
        }
        if( url.i_port != 0 )
        {
            /* msg_Info( p_stream, "Consider passing --rtsp-port=%u on "
                      "the command line instead.", url.i_port ); */
            var_Create( p_stream, "rtsp-port", VLC_VAR_INTEGER );
            var_SetInteger( p_stream, "rtsp-port", url.i_port );
        }
        p_sys->rtsp = RtspSetup( VLC_OBJECT(p_stream), NULL, url.psz_path );
        if( p_sys->rtsp == NULL )
            msg_Err( p_stream, "cannot export SDP as RTSP" );
    }
    else if( ( url.psz_protocol && !strcasecmp( url.psz_protocol, "sap" ) ) ||
             ( url.psz_host && !strcasecmp( url.psz_host, "sap" ) ) )
    {
        p_sys->b_export_sap = true;
        SapSetup( p_stream );
    }
    else if( url.psz_protocol && !strcasecmp( url.psz_protocol, "file" ) )
    {
        if( p_sys->psz_sdp_file != NULL )
        {
            msg_Err( p_stream, "you can use sdp=file:// only once" );
            goto out;
        }
        p_sys->psz_sdp_file = make_path( psz_url );
        if( p_sys->psz_sdp_file == NULL )
            goto out;
        FileSetup( p_stream );
    }
    else
    {
        msg_Warn( p_stream, "unknown protocol for SDP (%s)",
                  url.psz_protocol );
    }
out:
    vlc_UrlClean( &url );
}
