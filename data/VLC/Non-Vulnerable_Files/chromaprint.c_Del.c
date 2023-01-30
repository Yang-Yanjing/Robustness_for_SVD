}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    Finish( p_stream );
    if ( p_sys->id == id ) /* not assuming only 1 id is in use.. */
        p_sys->id = NULL;
    free( id );
    return VLC_SUCCESS;
}
