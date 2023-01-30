}
static int CreateBuffer( audio_output_t *aout, int size )
{
    audio_buffer_t *buffer;
    buffer = calloc( 1, sizeof( *buffer ));
    if( !buffer )
        return -1;
    buffer->data = malloc( size );
    if( !buffer->data )
    {
        free( buffer );
        return -1;
    }
    buffer->size = size;
    vlc_mutex_init( &buffer->mutex );
    vlc_cond_init( &buffer->cond );
    aout->sys->buffer = buffer;
    return 0;
}
