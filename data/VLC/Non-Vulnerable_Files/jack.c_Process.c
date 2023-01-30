 *****************************************************************************/
int Process( jack_nframes_t i_frames, void *p_arg )
{
    unsigned int i, j, frames_from_rb = 0;
    size_t bytes_read = 0;
    size_t frames_read;
    audio_output_t *p_aout = (audio_output_t*) p_arg;
    struct aout_sys_t *p_sys = p_aout->sys;
    /* Get the next audio data buffer unless paused */
    if( p_sys->paused == VLC_TS_INVALID )
        frames_from_rb = i_frames;
    /* Get the JACK buffers to write to */
    for( i = 0; i < p_sys->i_channels; i++ )
    {
        p_sys->p_jack_buffers[i] = jack_port_get_buffer( p_sys->p_jack_ports[i],
                                                         i_frames );
    }
    /* Copy in the audio data */
    for( j = 0; j < frames_from_rb; j++ )
    {
        for( i = 0; i < p_sys->i_channels; i++ )
        {
            jack_sample_t *p_dst = p_sys->p_jack_buffers[i] + j;
            bytes_read += jack_ringbuffer_read( p_sys->p_jack_ringbuffer,
                    (char *) p_dst, sizeof(jack_sample_t) );
        }
    }
    /* Fill any remaining buffer with silence */
    frames_read = (bytes_read / sizeof(jack_sample_t)) / p_sys->i_channels;
    if( frames_read < i_frames )
    {
        for( i = 0; i < p_sys->i_channels; i++ )
        {
            memset( p_sys->p_jack_buffers[i] + frames_read, 0,
                    sizeof( jack_sample_t ) * (i_frames - frames_read) );
        }
    }
    return 0;
}
