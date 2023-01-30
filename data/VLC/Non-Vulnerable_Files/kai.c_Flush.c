}
static void Flush( audio_output_t *aout, bool drain )
{
    audio_buffer_t *buffer = aout->sys->buffer;
    vlc_mutex_lock( &buffer->mutex );
    if( drain )
    {
        while( buffer->length > 0 )
            vlc_cond_wait( &buffer->cond, &buffer->mutex );
    }
    else
    {
        buffer->read_pos = buffer->write_pos;
        buffer->length   = 0;
    }
    vlc_mutex_unlock( &buffer->mutex );
}
