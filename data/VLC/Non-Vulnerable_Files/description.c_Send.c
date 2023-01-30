}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    VLC_UNUSED(id);
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    block_ChainRelease( p_buffer );
    if( p_sys->i_stream_start + 1500000 < mdate() )
        vlc_sem_post(p_sys->data->sem);
    return VLC_SUCCESS;
}
