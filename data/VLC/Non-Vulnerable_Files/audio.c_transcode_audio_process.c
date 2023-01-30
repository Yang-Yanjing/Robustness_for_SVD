}
int transcode_audio_process( sout_stream_t *p_stream,
                                    sout_stream_id_sys_t *id,
                                    block_t *in, block_t **out )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    block_t *p_block, *p_audio_buf;
    *out = NULL;
    if( unlikely( in == NULL ) )
    {
        block_t *p_block;
        do {
           p_block = id->p_encoder->pf_encode_audio(id->p_encoder, NULL );
           block_ChainAppend( out, p_block );
        } while( p_block );
        return VLC_SUCCESS;
    }
    while( (p_audio_buf = id->p_decoder->pf_decode_audio( id->p_decoder,
                                                          &in )) )
    {
        if( unlikely( !id->p_encoder->p_module ) )
        {
            /* Complete destination format */
            id->p_encoder->fmt_out.i_codec = p_sys->i_acodec;
            id->p_encoder->fmt_out.audio.i_rate = p_sys->i_sample_rate > 0 ?
                p_sys->i_sample_rate : id->p_decoder->fmt_out.audio.i_rate;
            id->p_encoder->fmt_out.i_bitrate = p_sys->i_abitrate;
            id->p_encoder->fmt_out.audio.i_bitspersample =
                id->p_decoder->fmt_out.audio.i_bitspersample;
            id->p_encoder->fmt_out.audio.i_channels = p_sys->i_channels > 0 ?
                p_sys->i_channels : id->p_decoder->fmt_out.audio.i_channels;
            id->p_encoder->fmt_in.audio.i_original_channels =
            id->p_encoder->fmt_out.audio.i_original_channels =
                id->p_decoder->fmt_out.audio.i_physical_channels;
            id->p_encoder->fmt_in.audio.i_physical_channels =
            id->p_encoder->fmt_out.audio.i_physical_channels =
                pi_channels_maps[id->p_encoder->fmt_out.audio.i_channels];
            if( transcode_audio_initialize_encoder( id, p_stream ) )
            {
                msg_Err( p_stream, "cannot create audio chain" );
                return VLC_EGENERIC;
            }
            if( unlikely( transcode_audio_initialize_filters( p_stream, id, p_sys,
                          &id->p_decoder->fmt_out.audio ) != VLC_SUCCESS ) )
                return VLC_EGENERIC;
            date_Init( &id->next_input_pts, id->p_decoder->fmt_out.audio.i_rate, 1 );
            date_Set( &id->next_input_pts, p_audio_buf->i_pts );
        }
        /* Check if audio format has changed, and filters need reinit */
        if( unlikely( ( id->p_decoder->fmt_out.audio.i_rate != id->fmt_audio.i_rate ) ||
                      ( id->p_decoder->fmt_out.audio.i_physical_channels != id->fmt_audio.i_physical_channels ) ) )
        {
            msg_Info( p_stream, "Audio changed, trying to reinitialize filters" );
            if( id->p_af_chain != NULL )
                aout_FiltersDelete( (vlc_object_t *)NULL, id->p_af_chain );
            /* decoders don't set audio.i_format, but audio filters use it */
            id->p_decoder->fmt_out.audio.i_format = id->p_decoder->fmt_out.i_codec;
            aout_FormatPrepare( &id->p_decoder->fmt_out.audio );
            if( transcode_audio_initialize_filters( p_stream, id, p_sys,
                          &id->p_decoder->fmt_out.audio ) != VLC_SUCCESS )
                return VLC_EGENERIC;
            /* Set next_input_pts to run with new samplerate */
            date_Init( &id->next_input_pts, id->fmt_audio.i_rate, 1 );
            date_Set( &id->next_input_pts, p_audio_buf->i_pts );
        }
        if( p_sys->b_master_sync )
        {
            mtime_t i_pts = date_Get( &id->next_input_pts );
            mtime_t i_drift = 0;
            if( likely( p_audio_buf->i_pts != VLC_TS_INVALID ) )
                i_drift = p_audio_buf->i_pts - i_pts;
            if ( unlikely(i_drift > MASTER_SYNC_MAX_DRIFT
                 || i_drift < -MASTER_SYNC_MAX_DRIFT) )
            {
                msg_Dbg( p_stream,
                    "audio drift is too high (%"PRId64"), resetting master sync",
                    i_drift );
                date_Set( &id->next_input_pts, p_audio_buf->i_pts );
                i_pts = date_Get( &id->next_input_pts );
                if( likely(p_audio_buf->i_pts != VLC_TS_INVALID ) )
                    i_drift = p_audio_buf->i_pts - i_pts;
            }
            p_sys->i_master_drift = i_drift;
            date_Increment( &id->next_input_pts, p_audio_buf->i_nb_samples );
        }
        p_audio_buf->i_dts = p_audio_buf->i_pts;
        /* Run filter chain */
        p_audio_buf = aout_FiltersPlay( id->p_af_chain, p_audio_buf,
                                        INPUT_RATE_DEFAULT );
        if( !p_audio_buf )
            abort();
        p_audio_buf->i_dts = p_audio_buf->i_pts;
        p_block = id->p_encoder->pf_encode_audio( id->p_encoder, p_audio_buf );
        block_ChainAppend( out, p_block );
        block_Release( p_audio_buf );
    }
    return VLC_SUCCESS;
}
