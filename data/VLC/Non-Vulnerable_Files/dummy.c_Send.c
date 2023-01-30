}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    (void)p_stream; (void)id;
    block_ChainRelease( p_buffer );
    return VLC_SUCCESS;
}
