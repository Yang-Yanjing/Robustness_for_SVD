 ****************************************************************************/
static int Start( access_t *p_access, uint64_t i_pos )
{
    access_sys_t *p_sys = p_access->p_sys;
    int  i_streams = 0;
    int  i_streams_selected = 0;
    int  i;
    char *psz = NULL;
    msg_Dbg( p_access, "starting stream" );
    for( i = 1; i < 128; i++ )
    {
        if( p_sys->asfh.stream[i].i_cat == ASF_CODEC_TYPE_UNKNOWN )
            continue;
        i_streams++;
        if( p_sys->asfh.stream[i].i_selected )
            i_streams_selected++;
    }
    if( i_streams_selected <= 0 )
    {
        msg_Err( p_access, "no stream selected" );
        return VLC_EGENERIC;
    }
    if( OpenConnection( p_access ) )
        return VLC_EGENERIC;
    net_Printf( p_access, p_sys->fd, NULL,
                "Accept: */*\r\n"
                "User-Agent: "MMSH_USER_AGENT"\r\n" );
    if( p_sys->b_broadcast )
    {
        net_Printf( p_access, p_sys->fd, NULL,
                    "Pragma: no-cache,rate=1.000000,request-context=%d\r\n",
                    p_sys->i_request_context++ );
    }
    else
    {
        net_Printf( p_access, p_sys->fd, NULL,
                    "Pragma: no-cache,rate=1.000000,stream-time=0,stream-offset=%u:%u,request-context=%d,max-duration=0\r\n",
                    (uint32_t)((i_pos >> 32)&0xffffffff),
                    (uint32_t)(i_pos&0xffffffff),
                    p_sys->i_request_context++ );
    }
    net_Printf( p_access, p_sys->fd, NULL,
                "Pragma: xPlayStrm=1\r\n"
                "Pragma: xClientGUID={"GUID_FMT"}\r\n"
                "Pragma: stream-switch-count=%d\r\n"
                "Pragma: stream-switch-entry=",
                GUID_PRINT( p_sys->guid ),
                i_streams);
    for( i = 1; i < 128; i++ )
    {
        if( p_sys->asfh.stream[i].i_cat != ASF_CODEC_TYPE_UNKNOWN )
        {
            int i_select = 2;
            if( p_sys->asfh.stream[i].i_selected )
            {
                i_select = 0;
            }
            net_Printf( p_access, p_sys->fd, NULL,
                        "ffff:%x:%d ", i, i_select );
        }
    }
    net_Printf( p_access, p_sys->fd, NULL, "\r\n" );
    net_Printf( p_access, p_sys->fd, NULL,
                "Connection: Close\r\n" );
    if( net_Printf( p_access, p_sys->fd, NULL, "\r\n" ) < 0 )
    {
        msg_Err( p_access, "failed to send request" );
        return VLC_EGENERIC;
    }
    psz = net_Gets( p_access, p_sys->fd, NULL );
    if( psz == NULL )
    {
        msg_Err( p_access, "cannot read data 0" );
        return VLC_EGENERIC;
    }
    if( atoi( &psz[9] ) >= 400 )
    {
        msg_Err( p_access, "error: %s", psz );
        free( psz );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_access, "HTTP reply '%s'", psz );
    free( psz );
    /* FIXME check HTTP code */
    for( ;; )
    {
        char *psz = net_Gets( p_access, p_sys->fd, NULL );
        if( psz == NULL )
        {
            msg_Err( p_access, "cannot read data 1" );
            return VLC_EGENERIC;
        }
        if( *psz == '\0' )
        {
            free( psz );
            break;
        }
        msg_Dbg( p_access, "%s", psz );
        free( psz );
    }
    p_sys->i_packet_used   = 0;
    p_sys->i_packet_length = 0;
    return VLC_SUCCESS;
}
