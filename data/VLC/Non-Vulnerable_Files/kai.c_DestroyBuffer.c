}
static void DestroyBuffer( audio_output_t *aout )
{
    audio_buffer_t *buffer = aout->sys->buffer;
    vlc_mutex_destroy( &buffer->mutex );
    vlc_cond_destroy( &buffer->cond );
    free( buffer->data );
    free( buffer );
}
