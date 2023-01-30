}
static void HandleAudioConfig( demux_t *p_demux, const uint16_t *p_anc,
                               uint8_t i_data_count, uint8_t i_group )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if ( i_data_count != 18 )
    {
        msg_Warn( p_demux, "malformed audio config for group %u", i_group );
        return;
    }
    for ( int i = 0; i < MAX_AUDIOS; i++ )
    {
        sdi_audio_t *p_audio = &p_sys->p_audios[i];
        if ( p_audio->i_group == i_group && p_audio->p_es == NULL )
        {
            unsigned int i_rate;
            if ( p_audio->i_pair == 2 )
            {
                i_rate = (p_anc[2] & 0xe0) >> 5;
                if ( p_anc[7] & 0x1 )
                {
                    uint32_t i_tmp = ((p_anc[7] & 0x1fe) >> 1)
                                       | ((p_anc[8] & 0x1ff) << 8)
                                       | ((p_anc[9] & 0x1ff) << 17);
                    if ( p_anc[9] & 0x80 )
                        p_audio->i_delay = i_tmp | 0xfc000000;
                    else
                        p_audio->i_delay = i_tmp;
                }
                if ( p_anc[13] & 0x1 )
                    msg_Warn( p_demux, "asymetric audio is not supported" );
            }
            else
            {
                i_rate = (p_anc[2] & 0xe) >> 1;
                if ( p_anc[4] & 0x1 )
                {
                    uint32_t i_tmp = ((p_anc[4] & 0x1fe) >> 1)
                                       | ((p_anc[5] & 0x1ff) << 8)
                                       | ((p_anc[6] & 0x1ff) << 17);
                    if ( p_anc[6] & 0x80 )
                        p_audio->i_delay = i_tmp | 0xfc000000;
                    else
                        p_audio->i_delay = i_tmp;
                }
                if ( p_anc[10] & 0x1 )
                    msg_Warn( p_demux, "asymetric audio is not supported" );
            }
            switch ( i_rate )
            {
            case 0: p_audio->i_rate = 48000; break;
            case 1: p_audio->i_rate = 44100; break;
            case 2: p_audio->i_rate = 32000; break;
            default:
                msg_Warn( p_demux, "unknown rate for audio %u/%u (%u)",
                          i_group, p_sys->p_audios[i].i_pair, i_rate );
                continue;
            }
            if( InitAudio( p_demux, p_audio ) != VLC_SUCCESS )
                return;
        }
    }
}
