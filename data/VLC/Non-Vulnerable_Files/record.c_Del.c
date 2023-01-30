}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    if( !p_sys->p_out )
        OutputStart( p_stream );
    if( id->p_first )
        block_ChainRelease( id->p_first );
    assert( !id->id || p_sys->p_out );
    if( id->id )
        sout_StreamIdDel( p_sys->p_out, id->id );
    es_format_Clean( &id->fmt );
    TAB_REMOVE( p_sys->i_id, p_sys->id, id );
    if( p_sys->i_id <= 0 )
    {
        if( !p_sys->p_out )
            p_sys->b_drop = false;
    }
    free( id );
    return VLC_SUCCESS;
}
