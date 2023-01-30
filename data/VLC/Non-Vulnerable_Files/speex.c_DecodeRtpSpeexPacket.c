}
static block_t *DecodeRtpSpeexPacket( decoder_t *p_dec, block_t **pp_block )
{
    block_t *p_speex_bit_block = *pp_block;
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_aout_buffer;
    int i_decode_ret;
    unsigned int i_speex_frame_size;
    if ( !p_speex_bit_block || p_speex_bit_block->i_pts <= VLC_TS_INVALID )
        return NULL;
    /*
      If the SpeexBits buffer size is 0 (a default value),
      we know that a proper initialization has not yet been done.
    */
    if ( p_sys->bits.buf_size==0 )
    {
        p_sys->p_header = malloc(sizeof(SpeexHeader));
        if ( !p_sys->p_header )
        {
            msg_Err( p_dec, "Could not allocate a Speex header.");
            return NULL;
        }
        const SpeexMode *mode = speex_lib_get_mode((p_sys->rtp_rate / 8000) >> 1);
        speex_init_header( p_sys->p_header,p_sys->rtp_rate, 1, mode );
        speex_bits_init( &p_sys->bits );
        p_sys->p_state = speex_decoder_init( mode );
        if ( !p_sys->p_state )
        {
            msg_Err( p_dec, "Could not allocate a Speex decoder." );
            free( p_sys->p_header );
            return NULL;
        }
        /*
          Assume that variable bit rate is enabled. Also assume
          that there is only one frame per packet.
        */
        p_sys->p_header->vbr = 1;
        p_sys->p_header->frames_per_packet = 1;
        p_dec->fmt_out.audio.i_channels = p_sys->p_header->nb_channels;
        p_dec->fmt_out.audio.i_physical_channels =
        p_dec->fmt_out.audio.i_original_channels =
            pi_channels_maps[p_sys->p_header->nb_channels];
        p_dec->fmt_out.audio.i_rate = p_sys->p_header->rate;
        if ( speex_mode_query( &speex_nb_mode,
                               SPEEX_MODE_FRAME_SIZE,
                               &i_speex_frame_size ) )
        {
            msg_Err( p_dec, "Could not determine the frame size." );
            speex_decoder_destroy( p_sys->p_state );
            free( p_sys->p_header );
            return NULL;
        }
        p_dec->fmt_out.audio.i_bytes_per_frame = i_speex_frame_size;
        date_Init(&p_sys->end_date, p_sys->p_header->rate, 1);
    }
    /*
      If the SpeexBits are initialized but there is
      still no header, an error must be thrown.
    */
    if ( !p_sys->p_header )
    {
        msg_Err( p_dec, "There is no valid Speex header found." );
        return NULL;
    }
    *pp_block = NULL;
    if ( !date_Get( &p_sys->end_date ) )
        date_Set( &p_sys->end_date, p_speex_bit_block->i_dts );
    /*
      Ask for a new audio output buffer and make sure
      we get one.
    */
    p_aout_buffer = decoder_NewAudioBuffer( p_dec,
        p_sys->p_header->frame_size );
    if ( !p_aout_buffer || p_aout_buffer->i_buffer == 0 )
    {
        msg_Err(p_dec, "Oops: No new buffer was returned!");
        return NULL;
    }
    /*
      Read the Speex payload into the SpeexBits buffer.
    */
    speex_bits_read_from( &p_sys->bits,
        (char*)p_speex_bit_block->p_buffer,
        p_speex_bit_block->i_buffer );
    /*
      Decode the input and ensure that no errors
      were encountered.
    */
    i_decode_ret = speex_decode_int( p_sys->p_state, &p_sys->bits,
            (int16_t*)p_aout_buffer->p_buffer );
    if ( i_decode_ret < 0 )
    {
        msg_Err( p_dec, "Decoding failed. Perhaps we have a bad stream?" );
        return NULL;
    }
    /*
      Handle date management on the audio output buffer.
    */
    p_aout_buffer->i_pts = date_Get( &p_sys->end_date );
    p_aout_buffer->i_length = date_Increment( &p_sys->end_date,
        p_sys->p_header->frame_size ) - p_aout_buffer->i_pts;
    p_sys->i_frame_in_packet++;
    block_Release( p_speex_bit_block );
    return p_aout_buffer;
}
