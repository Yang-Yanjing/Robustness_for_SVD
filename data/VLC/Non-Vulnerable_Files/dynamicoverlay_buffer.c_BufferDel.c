}
int BufferDel( buffer_t *p_buffer, int i_len )
{
    p_buffer->i_length -= i_len;
    if( p_buffer->i_length == 0 )
    {
        /* No data, we can reset the buffer now. */
        p_buffer->p_begin = p_buffer->p_memory;
    }
    else
    {
        p_buffer->p_begin += i_len;
    }
    return VLC_SUCCESS;
}
