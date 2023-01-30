}
static int WriteBuffer( audio_output_t *aout, uint8_t *data, int size )
{
    audio_buffer_t *buffer = aout->sys->buffer;
    int             len;
    int             remain_len = 0;
    vlc_mutex_lock( &buffer->mutex );
    /* FIXME :
     * If size is larger than buffer->size, this is locked indefinitely.
     */
    while( buffer->length + size > buffer->size )
        vlc_cond_wait( &buffer->cond, &buffer->mutex );
    len = size;
    if( buffer->write_pos + len > buffer->size )
    {
        remain_len  = len;
        len         = buffer->size - buffer->write_pos;
        remain_len -= len;
    }
    memcpy( buffer->data + buffer->write_pos, data, len );
    if( remain_len )
        memcpy( buffer->data, data + len, remain_len );
    buffer->write_pos += size;
    buffer->write_pos %= buffer->size;
    buffer->length += size;
    vlc_mutex_unlock( &buffer->mutex );
    return size;
}
