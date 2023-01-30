 *****************************************************************************/
static block_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block;
    if( !pp_block || !*pp_block ) return NULL;
    p_block = *pp_block;
    if( p_block->i_flags&(BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) )
    {
        block_Release( p_block );
        return NULL;
    }
    /* Remove ADTS header if we have decoder specific config */
    if( p_dec->fmt_in.i_extra && p_block->i_buffer > 7 )
    {
        if( p_block->p_buffer[0] == 0xff &&
            ( p_block->p_buffer[1] & 0xf0 ) == 0xf0 ) /* syncword */
        {   /* ADTS header present */
            size_t i_header_size; /* 7 bytes (+ 2 bytes for crc) */
            i_header_size = 7 + ( ( p_block->p_buffer[1] & 0x01 ) ? 0 : 2 );
            /* FIXME: multiple blocks per frame */
            if( p_block->i_buffer > i_header_size )
            {
                p_block->p_buffer += i_header_size;
                p_block->i_buffer -= i_header_size;
            }
        }
    }
    /* Append the block to the temporary buffer */
    if( p_sys->i_buffer_size < p_sys->i_buffer + p_block->i_buffer )
    {
        size_t  i_buffer_size = p_sys->i_buffer + p_block->i_buffer;
        uint8_t *p_buffer     = realloc( p_sys->p_buffer, i_buffer_size );
        if( p_buffer )
        {
            p_sys->i_buffer_size = i_buffer_size;
            p_sys->p_buffer      = p_buffer;
        }
        else
        {
            p_block->i_buffer = 0;
        }
    }
    if( p_block->i_buffer > 0 )
    {
        memcpy( &p_sys->p_buffer[p_sys->i_buffer],
                     p_block->p_buffer, p_block->i_buffer );
        p_sys->i_buffer += p_block->i_buffer;
        p_block->i_buffer = 0;
    }
    if( p_dec->fmt_out.audio.i_rate == 0 && p_dec->fmt_in.i_extra > 0 )
    {
        /* We have a decoder config so init the handle */
        unsigned long i_rate;
        unsigned char i_channels;
        if( faacDecInit2( p_sys->hfaad, p_dec->fmt_in.p_extra,
                          p_dec->fmt_in.i_extra,
                          &i_rate, &i_channels ) >= 0 )
        {
            p_dec->fmt_out.audio.i_rate = i_rate;
            p_dec->fmt_out.audio.i_channels = i_channels;
            p_dec->fmt_out.audio.i_physical_channels
                = p_dec->fmt_out.audio.i_original_channels
                = pi_channels_guessed[i_channels];
            date_Init( &p_sys->date, i_rate, 1 );
        }
    }
    if( p_dec->fmt_out.audio.i_rate == 0 && p_sys->i_buffer )
    {
        unsigned long i_rate;
        unsigned char i_channels;
        /* Init faad with the first frame */
        if( faacDecInit( p_sys->hfaad,
                         p_sys->p_buffer, p_sys->i_buffer,
                         &i_rate, &i_channels ) < 0 )
        {
            block_Release( p_block );
            return NULL;
        }
        p_dec->fmt_out.audio.i_rate = i_rate;
        p_dec->fmt_out.audio.i_channels = i_channels;
        p_dec->fmt_out.audio.i_physical_channels
            = p_dec->fmt_out.audio.i_original_channels
            = pi_channels_guessed[i_channels];
        date_Init( &p_sys->date, i_rate, 1 );
    }
    if( p_block->i_pts > VLC_TS_INVALID && p_block->i_pts != date_Get( &p_sys->date ) )
    {
        date_Set( &p_sys->date, p_block->i_pts );
    }
    else if( !date_Get( &p_sys->date ) )
    {
        /* We've just started the stream, wait for the first PTS. */
        block_Release( p_block );
        p_sys->i_buffer = 0;
        return NULL;
    }
    /* Decode all data */
    if( p_sys->i_buffer > 1)
    {
        void *samples;
        faacDecFrameInfo frame;
        block_t *p_out;
        samples = faacDecDecode( p_sys->hfaad, &frame,
                                 p_sys->p_buffer, p_sys->i_buffer );
        if( frame.error > 0 )
        {
            msg_Warn( p_dec, "%s", faacDecGetErrorMessage( frame.error ) );
            if( frame.error == 21 || frame.error == 12 )
            {
                /*
                 * Once an "Unexpected channel configuration change"
                 * or a "Invalid number of channels" error
                 * occurs, it will occurs afterwards, and we got no sound.
                 * Reinitialization of the decoder is required.
                 */
                unsigned long i_rate;
                unsigned char i_channels;
                faacDecHandle *hfaad;
                faacDecConfiguration *cfg,*oldcfg;
                oldcfg = faacDecGetCurrentConfiguration( p_sys->hfaad );
                hfaad = faacDecOpen();
                cfg = faacDecGetCurrentConfiguration( hfaad );
                if( oldcfg->defSampleRate )
                    cfg->defSampleRate = oldcfg->defSampleRate;
                cfg->defObjectType = oldcfg->defObjectType;
                cfg->outputFormat = oldcfg->outputFormat;
                faacDecSetConfiguration( hfaad, cfg );
                if( faacDecInit( hfaad, p_sys->p_buffer, p_sys->i_buffer,
                                &i_rate,&i_channels ) < 0 )
                {
                    /* reinitialization failed */
                    faacDecClose( hfaad );
                    faacDecSetConfiguration( p_sys->hfaad, oldcfg );
                }
                else
                {
                    faacDecClose( p_sys->hfaad );
                    p_sys->hfaad = hfaad;
                    p_dec->fmt_out.audio.i_rate = i_rate;
                    p_dec->fmt_out.audio.i_channels = i_channels;
                    p_dec->fmt_out.audio.i_physical_channels
                        = p_dec->fmt_out.audio.i_original_channels
                        = pi_channels_guessed[i_channels];
                    date_Init( &p_sys->date, i_rate, 1 );
                }
            }
            /* Flush the buffer */
            p_sys->i_buffer = 0;
            block_Release( p_block );
            return NULL;
        }
        if( frame.channels <= 0 || frame.channels > 8 || frame.channels == 7 )
        {
            msg_Warn( p_dec, "invalid channels count: %i", frame.channels );
            /* Flush the buffer */
            p_sys->i_buffer -= frame.bytesconsumed;
            if( p_sys->i_buffer > 0 )
            {
                memmove( p_sys->p_buffer,&p_sys->p_buffer[frame.bytesconsumed],
                         p_sys->i_buffer );
            }
            block_Release( p_block );
            return NULL;
        }
        if( frame.samples <= 0 )
        {
            msg_Warn( p_dec, "decoded zero sample" );
            /* Flush the buffer */
            p_sys->i_buffer -= frame.bytesconsumed;
            if( p_sys->i_buffer > 1 )
            {
                memmove( p_sys->p_buffer,&p_sys->p_buffer[frame.bytesconsumed],
                         p_sys->i_buffer );
            }
            else
            {
                /* Drop byte of padding */
                p_sys->i_buffer = 0;
            }
            block_Release( p_block );
            return NULL;
        }
        /* We decoded a valid frame */
        if( p_dec->fmt_out.audio.i_rate != frame.samplerate )
        {
            date_Init( &p_sys->date, frame.samplerate, 1 );
            date_Set( &p_sys->date, p_block->i_pts );
        }
        p_block->i_pts = VLC_TS_INVALID;  /* PTS is valid only once */
        p_dec->fmt_out.audio.i_rate = frame.samplerate;
        p_dec->fmt_out.audio.i_channels = frame.channels;
        /* Adjust stream info when dealing with SBR/PS */
        bool b_sbr = (frame.sbr == 1) || (frame.sbr == 2);
        if( p_sys->b_sbr != b_sbr || p_sys->b_ps != frame.ps )
        {
            const char *psz_ext = (b_sbr && frame.ps) ? "SBR+PS" :
                                    b_sbr ? "SBR" : "PS";
            msg_Dbg( p_dec, "AAC %s (channels: %u, samplerate: %lu)",
                    psz_ext, frame.channels, frame.samplerate );
            if( !p_dec->p_description )
                p_dec->p_description = vlc_meta_New();
            if( p_dec->p_description )
                vlc_meta_AddExtra( p_dec->p_description, _("AAC extension"), psz_ext );
            p_sys->b_sbr = b_sbr;
            p_sys->b_ps = frame.ps;
        }
        /* Convert frame.channel_position to our own channel values */
        p_dec->fmt_out.audio.i_physical_channels = 0;
        const uint32_t nbChannels = frame.channels;
        unsigned j;
        for( unsigned i = 0; i < nbChannels; i++ )
        {
            /* Find the channel code */
            for( j = 0; j < MAX_CHANNEL_POSITIONS; j++ )
            {
                if( frame.channel_position[i] == pi_channels_in[j] )
                    break;
            }
            if( j >= MAX_CHANNEL_POSITIONS )
            {
                msg_Warn( p_dec, "unknown channel ordering" );
                /* Invent something */
                j = i;
            }
            /* */
            p_sys->pi_channel_positions[i] = pi_channels_out[j];
            if( p_dec->fmt_out.audio.i_physical_channels & pi_channels_out[j] )
                frame.channels--; /* We loose a duplicated channel */
            else
                p_dec->fmt_out.audio.i_physical_channels |= pi_channels_out[j];
        }
        if ( nbChannels != frame.channels )
        {
            p_dec->fmt_out.audio.i_physical_channels
                = p_dec->fmt_out.audio.i_original_channels
                = pi_channels_guessed[nbChannels];
        }
        else
        {
            p_dec->fmt_out.audio.i_original_channels =
                p_dec->fmt_out.audio.i_physical_channels;
        }
        p_dec->fmt_out.audio.i_channels = nbChannels;
        p_out = decoder_NewAudioBuffer( p_dec, frame.samples / nbChannels );
        if( p_out == NULL )
        {
            p_sys->i_buffer = 0;
            block_Release( p_block );
            return NULL;
        }
        p_out->i_pts = date_Get( &p_sys->date );
        p_out->i_length = date_Increment( &p_sys->date,
                                          frame.samples / nbChannels )
                          - p_out->i_pts;
        DoReordering( (uint32_t *)p_out->p_buffer, samples,
                      frame.samples / nbChannels, nbChannels,
                      p_sys->pi_channel_positions );
        p_sys->i_buffer -= frame.bytesconsumed;
        if( p_sys->i_buffer > 0 )
        {
            memmove( p_sys->p_buffer, &p_sys->p_buffer[frame.bytesconsumed],
                     p_sys->i_buffer );
        }
        return p_out;
    }
    else
    {
        /* Drop byte of padding */
        p_sys->i_buffer = 0;
    }
    block_Release( p_block );
    return NULL;
}
