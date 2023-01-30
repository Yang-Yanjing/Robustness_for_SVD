}
static int HandleAudio( demux_t *p_demux, const uint8_t *p_buffer )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    for ( int i = 0; i < MAX_AUDIOS; i++ )
    {
        hdsdi_audio_t *p_audio = &p_sys->p_audios[i];
        if ( p_audio->i_channel != -1 && p_audio->p_es != NULL )
        {
            block_t *p_block = block_Alloc( p_sys->i_ablock_size );
            if( unlikely( !p_block ) )
                return VLC_ENOMEM;
            SparseCopy( (int16_t *)p_block->p_buffer, (const int16_t *)p_buffer,
                        p_sys->i_ablock_size / 4,
                        p_audio->i_channel * 2, p_sys->i_max_channel + 1 );
            p_block->i_dts = p_block->i_pts
                = p_sys->i_next_adate + (mtime_t)p_audio->i_delay
                   * INT64_C(1000000) / p_sys->i_sample_rate;
            p_block->i_length = p_sys->i_aincr;
            es_out_Send( p_demux->out, p_audio->p_es, p_block );
        }
    }
    p_sys->i_next_adate += p_sys->i_aincr;
    return VLC_SUCCESS;
}
