 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    uint8_t      hdr[20];
    const uint8_t *p_peek;
    int          i_cat;
    int          i_samples, i_modulo;
    if( stream_Peek( p_demux->s , &p_peek, 4 ) < 4 )
        return VLC_EGENERIC;
    if( memcmp( p_peek, ".snd", 4 ) )
        return VLC_EGENERIC;
    /* skip signature */
    stream_Read( p_demux->s, NULL, 4 );   /* cannot fail */
    /* read header */
    if( stream_Read( p_demux->s, hdr, 20 ) < 20 )
    {
        msg_Err( p_demux, "cannot read" );
        return VLC_EGENERIC;
    }
    if( GetDWBE( &hdr[0]  ) < 24 )
    {
        msg_Err( p_demux, "invalid file" );
        return VLC_EGENERIC;
    }
    DEMUX_INIT_COMMON(); p_sys = p_demux->p_sys;
    p_sys->i_time = 0;
    p_sys->i_header_size = GetDWBE( &hdr[0] );
    /* skip extra header data */
    if( p_sys->i_header_size > 24 )
    {
        stream_Read( p_demux->s, NULL, p_sys->i_header_size - 24 );
    }
    /* init fmt */
    es_format_Init( &p_sys->fmt, AUDIO_ES, 0 );
    p_sys->fmt.audio.i_rate     = GetDWBE( &hdr[12] );
    p_sys->fmt.audio.i_channels = GetDWBE( &hdr[16] );
#if 0
    p_sys->au.i_header_size   = GetDWBE( &p_sys->au.i_header_size );
    p_sys->au.i_data_size     = GetDWBE( &p_sys->au.i_data_size );
    p_sys->au.i_encoding      = GetDWBE( &p_sys->au.i_encoding );
    p_sys->au.i_sample_rate   = GetDWBE( &p_sys->au.i_sample_rate );
    p_sys->au.i_channels      = GetDWBE( &p_sys->au.i_channels );
#endif
    switch( GetDWBE( &hdr[8] ) )
    {
        case AU_ALAW_8:        /* 8-bit ISDN A-law */
            p_sys->fmt.i_codec               = VLC_CODEC_ALAW;
            p_sys->fmt.audio.i_bitspersample = 8;
            p_sys->fmt.audio.i_blockalign    = 1 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_PCM;
            break;
        case AU_MULAW_8:       /* 8-bit ISDN u-law */
            p_sys->fmt.i_codec               = VLC_CODEC_MULAW;
            p_sys->fmt.audio.i_bitspersample = 8;
            p_sys->fmt.audio.i_blockalign    = 1 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_PCM;
            break;
        case AU_LINEAR_8:      /* 8-bit linear PCM */
            p_sys->fmt.i_codec               = VLC_FOURCC( 't','w','o','s' );
            p_sys->fmt.audio.i_bitspersample = 8;
            p_sys->fmt.audio.i_blockalign    = 1 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_PCM;
            break;
        case AU_LINEAR_16:     /* 16-bit linear PCM */
            p_sys->fmt.i_codec               = VLC_FOURCC( 't','w','o','s' );
            p_sys->fmt.audio.i_bitspersample = 16;
            p_sys->fmt.audio.i_blockalign    = 2 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_PCM;
            break;
        case AU_LINEAR_24:     /* 24-bit linear PCM */
            p_sys->fmt.i_codec               = VLC_FOURCC( 't','w','o','s' );
            p_sys->fmt.audio.i_bitspersample = 24;
            p_sys->fmt.audio.i_blockalign    = 3 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_PCM;
            break;
        case AU_LINEAR_32:     /* 32-bit linear PCM */
            p_sys->fmt.i_codec               = VLC_FOURCC( 't','w','o','s' );
            p_sys->fmt.audio.i_bitspersample = 32;
            p_sys->fmt.audio.i_blockalign    = 4 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_PCM;
            break;
        case AU_FLOAT:         /* 32-bit IEEE floating point */
            p_sys->fmt.i_codec               = VLC_FOURCC( 'a', 'u', 0, AU_FLOAT );
            p_sys->fmt.audio.i_bitspersample = 32;
            p_sys->fmt.audio.i_blockalign    = 4 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_PCM;
            break;
        case AU_DOUBLE:        /* 64-bit IEEE floating point */
            p_sys->fmt.i_codec               = VLC_FOURCC( 'a', 'u', 0, AU_DOUBLE );
            p_sys->fmt.audio.i_bitspersample = 64;
            p_sys->fmt.audio.i_blockalign    = 8 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_PCM;
            break;
        case AU_ADPCM_G721:    /* 4-bit CCITT g.721 ADPCM */
            p_sys->fmt.i_codec               = VLC_FOURCC( 'a', 'u', 0, AU_ADPCM_G721 );
            p_sys->fmt.audio.i_bitspersample = 0;
            p_sys->fmt.audio.i_blockalign    = 0 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_ADPCM;
            break;
        case AU_ADPCM_G722:    /* CCITT g.722 ADPCM */
            p_sys->fmt.i_codec               = VLC_FOURCC( 'a', 'u', 0, AU_ADPCM_G722 );
            p_sys->fmt.audio.i_bitspersample = 0;
            p_sys->fmt.audio.i_blockalign    = 0 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_ADPCM;
            break;
        case AU_ADPCM_G723_3:  /* CCITT g.723 3-bit ADPCM */
            p_sys->fmt.i_codec               = VLC_FOURCC( 'a', 'u', 0, AU_ADPCM_G723_3 );
            p_sys->fmt.audio.i_bitspersample = 0;
            p_sys->fmt.audio.i_blockalign    = 0 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_ADPCM;
            break;
        case AU_ADPCM_G723_5:  /* CCITT g.723 5-bit ADPCM */
            p_sys->fmt.i_codec               = VLC_FOURCC( 'a', 'u', 0, AU_ADPCM_G723_5 );
            p_sys->fmt.audio.i_bitspersample = 0;
            p_sys->fmt.audio.i_blockalign    = 0 * p_sys->fmt.audio.i_channels;
            i_cat                    = AU_CAT_ADPCM;
            break;
        default:
            msg_Warn( p_demux, "unknow encoding=0x%x", GetDWBE( &hdr[8] ) );
            p_sys->fmt.audio.i_bitspersample = 0;
            p_sys->fmt.audio.i_blockalign    = 0;
            i_cat                    = AU_CAT_UNKNOWN;
            break;
    }
    p_sys->fmt.i_bitrate = p_sys->fmt.audio.i_rate *
                           p_sys->fmt.audio.i_channels *
                           p_sys->fmt.audio.i_bitspersample;
    if( i_cat == AU_CAT_UNKNOWN || i_cat == AU_CAT_ADPCM )
    {
        p_sys->i_frame_size = 0;
        p_sys->i_frame_length = 0;
        msg_Err( p_demux, "unsupported codec/type (Please report it)" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    if( p_sys->fmt.audio.i_rate == 0 )
    {
        msg_Err( p_demux, "invalid samplerate: 0" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    /* add the es */
    p_sys->es = es_out_Add( p_demux->out, &p_sys->fmt );
    /* calculate 50ms frame size/time */
    i_samples = __MAX( p_sys->fmt.audio.i_rate / 20, 1 );
    p_sys->i_frame_size = i_samples * p_sys->fmt.audio.i_channels *
                          ( (p_sys->fmt.audio.i_bitspersample + 7) / 8 );
    if( p_sys->fmt.audio.i_blockalign > 0 )
    {
        if( ( i_modulo = p_sys->i_frame_size % p_sys->fmt.audio.i_blockalign ) != 0 )
        {
            p_sys->i_frame_size += p_sys->fmt.audio.i_blockalign - i_modulo;
        }
    }
    p_sys->i_frame_length = (mtime_t)1000000 *
                            (mtime_t)i_samples /
                            (mtime_t)p_sys->fmt.audio.i_rate;
    return VLC_SUCCESS;
}
