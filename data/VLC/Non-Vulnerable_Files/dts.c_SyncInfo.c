};
static int SyncInfo( const uint8_t *p_buf,
                     bool *pb_dts_hd,
                     unsigned int *pi_channels,
                     unsigned int *pi_channels_conf,
                     unsigned int *pi_sample_rate,
                     unsigned int *pi_bit_rate,
                     unsigned int *pi_frame_length )
{
    unsigned int i_audio_mode;
    unsigned int i_frame_size = GetSyncInfo( p_buf, pb_dts_hd,
            pi_sample_rate, pi_bit_rate, pi_frame_length, &i_audio_mode);
    if( *pb_dts_hd == true )
        return i_frame_size;
    switch( i_audio_mode & 0xFFFF )
    {
        case 0x0:
            /* Mono */
            *pi_channels = 1;
            *pi_channels_conf = AOUT_CHAN_CENTER;
            break;
        case 0x1:
            /* Dual-mono = stereo + dual-mono */
            *pi_channels = 2;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT |
                           AOUT_CHAN_DUALMONO;
            break;
        case 0x2:
        case 0x3:
        case 0x4:
            /* Stereo */
            *pi_channels = 2;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT;
            break;
        case 0x5:
            /* 3F */
            *pi_channels = 3;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT |
                                AOUT_CHAN_CENTER;
            break;
        case 0x6:
            /* 2F/1R */
            *pi_channels = 3;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT |
                                AOUT_CHAN_REARCENTER;
            break;
        case 0x7:
            /* 3F/1R */
            *pi_channels = 4;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT |
                                AOUT_CHAN_CENTER | AOUT_CHAN_REARCENTER;
            break;
        case 0x8:
            /* 2F2R */
            *pi_channels = 4;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT |
                                AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT;
            break;
        case 0x9:
            /* 3F2R */
            *pi_channels = 5;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT |
                                AOUT_CHAN_CENTER | AOUT_CHAN_REARLEFT |
                                AOUT_CHAN_REARRIGHT;
            break;
        case 0xA:
        case 0xB:
            /* 2F2M2R */
            *pi_channels = 6;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT |
                                AOUT_CHAN_MIDDLELEFT | AOUT_CHAN_MIDDLERIGHT |
                                AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT;
            break;
        case 0xC:
            /* 3F2M2R */
            *pi_channels = 7;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT |
                                AOUT_CHAN_CENTER | AOUT_CHAN_MIDDLELEFT |
                                AOUT_CHAN_MIDDLERIGHT | AOUT_CHAN_REARLEFT |
                                AOUT_CHAN_REARRIGHT;
            break;
        case 0xD:
        case 0xE:
            /* 3F2M2R/LFE */
            *pi_channels = 8;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT |
                                AOUT_CHAN_CENTER | AOUT_CHAN_MIDDLELEFT |
                                AOUT_CHAN_MIDDLERIGHT | AOUT_CHAN_REARLEFT |
                                AOUT_CHAN_REARRIGHT | AOUT_CHAN_LFE;
            break;
        case 0xF:
        default:
            if( (i_audio_mode & 0xFFFF) >= 0x10 )
            {
                /* User defined */
                *pi_channels = 0;
                *pi_channels_conf = 0;
            }
            else return 0;
            break;
    }
    if( *pi_channels && (i_audio_mode & 0x10000) )
    {
        (*pi_channels)++;
        *pi_channels_conf |= AOUT_CHAN_LFE;
    }
    if( *pi_sample_rate >= sizeof( ppi_dts_samplerate ) /
                           sizeof( ppi_dts_samplerate[0] ) )
    {
        return 0;
    }
    *pi_sample_rate = ppi_dts_samplerate[ *pi_sample_rate ];
    if( !*pi_sample_rate ) return 0;
    if( *pi_bit_rate >= sizeof( ppi_dts_bitrate ) /
                        sizeof( ppi_dts_bitrate[0] ) )
    {
        return 0;
    }
    *pi_bit_rate = ppi_dts_bitrate[ *pi_bit_rate ];
    if( !*pi_bit_rate ) return 0;
    *pi_frame_length = (*pi_frame_length + 1) * 32;
    return i_frame_size;
}
