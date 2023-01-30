}
static int TimeGet( audio_output_t *aout, mtime_t *restrict delay )
{
    aout_sys_t            *sys = aout->sys;
    audio_sample_format_t *format = &sys->format;
    audio_buffer_t        *buffer = sys->buffer;
    vlc_mutex_lock( &buffer->mutex );
    *delay = ( buffer->length / format->i_bytes_per_frame ) * CLOCK_FREQ /
             format->i_rate;
    vlc_mutex_unlock( &buffer->mutex );
    return 0;
}
