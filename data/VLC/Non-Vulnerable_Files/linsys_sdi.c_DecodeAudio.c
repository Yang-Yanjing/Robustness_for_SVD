}
static int DecodeAudio( demux_t *p_demux, sdi_audio_t *p_audio )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t *p_block;
    int16_t *p_output;
    if ( p_audio->p_buffer == NULL )
        return VLC_EGENERIC;
    if ( !p_audio->i_left_samples && !p_audio->i_right_samples )
    {
        msg_Warn( p_demux, "no audio %u/%u", p_audio->i_group,
                  p_audio->i_pair );
        return VLC_EGENERIC;
    }
    if ( p_audio->i_left_samples <
            (float)p_audio->i_nb_samples * (1. - SAMPLERATE_TOLERANCE) ||
        p_audio->i_left_samples >
            (float)p_audio->i_nb_samples * (1. + SAMPLERATE_TOLERANCE) )
    {
        msg_Warn( p_demux,
            "left samplerate out of tolerance for audio %u/%u (%u vs. %u)",
            p_audio->i_group, p_audio->i_pair,
            p_audio->i_left_samples, p_audio->i_nb_samples );
        return VLC_EGENERIC;
    }
    if ( p_audio->i_right_samples <
            (float)p_audio->i_nb_samples * (1. - SAMPLERATE_TOLERANCE) ||
        p_audio->i_right_samples >
            (float)p_audio->i_nb_samples * (1. + SAMPLERATE_TOLERANCE) )
    {
        msg_Warn( p_demux,
            "right samplerate out of tolerance for audio %u/%u (%u vs. %u)",
            p_audio->i_group, p_audio->i_pair,
            p_audio->i_right_samples, p_audio->i_nb_samples );
        return VLC_EGENERIC;
    }
    p_block = block_Alloc( p_audio->i_nb_samples * sizeof(int16_t) * 2 );
    if( unlikely( !p_block ) )
        return VLC_ENOMEM;
    p_block->i_dts = p_block->i_pts = p_sys->i_next_date
        + (mtime_t)p_audio->i_delay * INT64_C(1000000) / p_audio->i_rate;
    p_output = (int16_t *)p_block->p_buffer;
    if ( p_audio->i_left_samples == p_audio->i_nb_samples &&
         p_audio->i_right_samples == p_audio->i_nb_samples )
        memcpy( p_output, p_audio->p_buffer,
                    p_audio->i_nb_samples * sizeof(int16_t) * 2 );
    else
    {
        ResampleAudio( p_output, p_audio->p_buffer,
                       p_audio->i_nb_samples, p_audio->i_left_samples );
        ResampleAudio( p_output + 1, p_audio->p_buffer + 1,
                       p_audio->i_nb_samples, p_audio->i_right_samples );
    }
    es_out_Send( p_demux->out, p_audio->p_es, p_block );
    return VLC_SUCCESS;
}
