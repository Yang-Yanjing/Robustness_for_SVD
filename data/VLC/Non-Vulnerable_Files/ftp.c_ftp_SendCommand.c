    ((access_sys_t *)(((sout_access_out_t *)(p_this))->p_sys))
static int ftp_SendCommand( vlc_object_t *obj, access_sys_t *sys,
                            const char *fmt, ... )
{
    size_t fmtlen = strlen( fmt );
    char fmtbuf[fmtlen + 3];
    memcpy( fmtbuf, fmt, fmtlen );
    memcpy( fmtbuf + fmtlen, "\r\n", 3 );
    va_list args;
    char *cmd;
    int val;
    va_start( args, fmt );
    val = vasprintf( &cmd, fmtbuf, args );
    va_end( args );
    if( unlikely(val == -1) )
        return -1;
    msg_Dbg( obj, "sending request: \"%.*s\" (%d bytes)", val - 2, cmd, val );
    if( net_Write( obj, sys->cmd.fd, sys->cmd.p_vs, cmd, val ) != val )
    {
        msg_Err( obj, "request failure" );
        val = -1;
    }
    else
        val = 0;
    free( cmd );
    return val;
}
