}
static void HandleAudioData( demux_t *p_demux, const uint16_t *p_anc,
                             uint8_t i_data_count, uint8_t i_group,
                             uint8_t i_block_number )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if ( i_data_count % 3 )
    {
        msg_Warn( p_demux, "malformed audio data for group %u", i_group );
        return;
    }
    for ( int i = 0; i < MAX_AUDIOS; i++ )
    {
        sdi_audio_t *p_audio = &p_sys->p_audios[i];
        if ( p_audio->i_group == i_group )
        {
            const uint16_t *x = p_anc;
            /* SMPTE 272M says that when parsing a frame, if an audio config
             * structure is present we will encounter it first. Otherwise
             * it is assumed to be 48 kHz. */
            if ( p_audio->p_es == NULL )
            {
                p_audio->i_rate = 48000;
                p_audio->i_delay = 0;
                if( InitAudio( p_demux, p_audio ) != VLC_SUCCESS )
                    return;
            }
            if ( i_block_number )
            {
                if ( p_audio->i_block_number + 1 != i_block_number )
                    msg_Warn( p_demux,
                              "audio data block discontinuity (%"PRIu8"->%"PRIu8") for group %"PRIu8,
                              p_audio->i_block_number, i_block_number,
                              i_group );
                if ( i_block_number == 0xff )
                    p_audio->i_block_number = 0;
                else
                    p_audio->i_block_number = i_block_number;
            }
            while ( x < p_anc + i_data_count )
            {
                if ( ((*x & 0x4) && p_audio->i_pair == 2)
                      || (!(*x & 0x4) && p_audio->i_pair == 1) )
                {
                    uint32_t i_tmp = (uint32_t)((x[0] & 0x1f1) >> 3)
                                                  | ((x[1] & 0x1ff) << 6)
                                                  | ((x[2] & 0x1f) << 15);
                    int32_t i_sample;
                    if ( x[2] & 0x10 )
                        i_sample = i_tmp | 0xfff00000;
                    else
                        i_sample = i_tmp;
                    if ( x[0] & 0x2 )
                    {
                        if ( p_audio->i_right_samples < p_audio->i_max_samples )
                            p_audio->p_buffer[2 * p_audio->i_right_samples
                                               + 1] = (i_sample + 8) / 16;
                        p_audio->i_right_samples++;
                    }
                    else
                    {
                        if ( p_audio->i_left_samples < p_audio->i_max_samples )
                            p_audio->p_buffer[2 * p_audio->i_left_samples]
                                = (i_sample + 8) / 16;
                        p_audio->i_left_samples++;
                    }
                }
                x += 3;
            }
        }
    }
}
