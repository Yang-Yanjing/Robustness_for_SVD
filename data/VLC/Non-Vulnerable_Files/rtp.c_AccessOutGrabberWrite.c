}
static ssize_t AccessOutGrabberWrite( sout_access_out_t *p_access,
                                      block_t *p_buffer )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_access->p_sys;
    while( p_buffer )
    {
        block_t *p_next;
        AccessOutGrabberWriteBuffer( p_stream, p_buffer );
        p_next = p_buffer->p_next;
        block_Release( p_buffer );
        p_buffer = p_next;
    }
    return VLC_SUCCESS;
}
