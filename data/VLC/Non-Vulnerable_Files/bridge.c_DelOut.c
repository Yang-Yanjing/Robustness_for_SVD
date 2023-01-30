}
static int DelOut( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    VLC_UNUSED(id);
    out_sout_stream_sys_t *p_sys = (out_sout_stream_sys_t *)p_stream->p_sys;
    bridged_es_t *p_es;
    if ( !p_sys->b_inited )
    {
        return VLC_SUCCESS;
    }
    vlc_mutex_lock( &lock );
    p_es = p_sys->p_es;
    p_es->b_empty = true;
    block_ChainRelease( p_es->p_block );
    p_es->p_block = false;
    p_es->b_changed = true;
    vlc_mutex_unlock( &lock );
    p_sys->b_inited = false;
    return VLC_SUCCESS;
}
