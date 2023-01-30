}
static int ReadBuffer( audio_output_t *aout, uint8_t *data, int size )
{
    audio_buffer_t *buffer = aout->sys->buffer;
    int             len;
    int             remain_len = 0;
    vlc_mutex_lock( &buffer->mutex );
    len = MIN( buffer->length, size );
    if( buffer->read_pos + len > buffer->size )
    {
        remain_len  = len;
        len         = buffer->size - buffer->read_pos;
        remain_len -= len;
    }
    memcpy( data, buffer->data + buffer->read_pos, len );
    if( remain_len )
        memcpy( data + len, buffer->data, remain_len );
    len += remain_len;
    buffer->read_pos += len;
    buffer->read_pos %= buffer->size;
    buffer->length -= len;
    vlc_cond_signal( &buffer->cond );
    vlc_mutex_unlock( &buffer->mutex );
    return len;
}
