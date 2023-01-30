}
static int SendOut( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                    block_t *p_buffer )
{
    out_sout_stream_sys_t *p_sys = (out_sout_stream_sys_t *)p_stream->p_sys;
    bridged_es_t *p_es;
    if ( (out_sout_stream_sys_t *)id != p_sys )
    {
        block_ChainRelease( p_buffer );
        return VLC_SUCCESS;
    }
    vlc_mutex_lock( &lock );
    p_es = p_sys->p_es;
    *p_es->pp_last = p_buffer;
    while ( p_buffer != NULL )
    {
        p_es->pp_last = &p_buffer->p_next;
        p_buffer = p_buffer->p_next;
    }
    vlc_mutex_unlock( &lock );
    return VLC_SUCCESS;
}
