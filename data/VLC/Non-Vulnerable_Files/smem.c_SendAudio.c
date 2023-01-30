}
static int SendAudio( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                      block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    int i_size;
    uint8_t* p_pcm_buffer = NULL;
    int i_samples = 0;
    i_size = p_buffer->i_buffer;
    if (id->format->audio.i_channels <= 0)
    {
        msg_Warn( p_stream, "No buffer given!" );
        block_ChainRelease( p_buffer );
        return VLC_EGENERIC;
    }
    i_samples = i_size / ( ( id->format->audio.i_bitspersample / 8 ) * id->format->audio.i_channels );
    /* Calling the prerender callback to get user buffer */
    p_sys->pf_audio_prerender_callback( id->p_data, &p_pcm_buffer, i_size );
    if (!p_pcm_buffer)
    {
        msg_Err( p_stream, "No buffer given!" );
        block_ChainRelease( p_buffer );
        return VLC_EGENERIC;
    }
    /* Copying data into user buffer */
    memcpy( p_pcm_buffer, p_buffer->p_buffer, i_size );
    /* Calling the postrender callback to tell the user his buffer is ready */
    p_sys->pf_audio_postrender_callback( id->p_data, p_pcm_buffer,
                                         id->format->audio.i_channels, id->format->audio.i_rate, i_samples,
                                         id->format->audio.i_bitspersample, i_size, p_buffer->i_pts );
    block_ChainRelease( p_buffer );
    return VLC_SUCCESS;
}
