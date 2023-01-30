}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    msg_Dbg( p_stream, "Removing a stream" );
    free( id );
    return VLC_SUCCESS;
}
