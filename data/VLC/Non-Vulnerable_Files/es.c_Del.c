}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    VLC_UNUSED(p_stream);
    sout_access_out_t *p_access = id->p_mux->p_access;
    sout_MuxDeleteStream( id->p_mux, id->p_input );
    sout_MuxDelete( id->p_mux );
    if( !sout_AccessOutCanControlPace( p_access ) )
        p_stream->p_sout->i_out_pace_nocontrol--;
    sout_AccessOutDelete( p_access );
    free( id );
    return VLC_SUCCESS;
}
