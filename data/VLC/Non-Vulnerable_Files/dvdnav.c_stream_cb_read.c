}
static int stream_cb_read( void *s, void* buffer, int size )
{
    return stream_Read( (stream_t *)s, buffer, size );
}
