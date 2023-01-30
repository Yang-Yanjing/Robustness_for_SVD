 *****************************************************************************/
static block_t *DecodePacket( decoder_t *p_dec, ogg_packet *p_oggpacket )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( p_oggpacket->bytes )
    {
        /* Copy Ogg packet to Speex bitstream */
        speex_bits_read_from( &p_sys->bits, (char *)p_oggpacket->packet,
                              p_oggpacket->bytes );
        p_sys->i_frame_in_packet = 0;
    }
    /* Decode one frame at a time */
    if( p_sys->i_frame_in_packet < p_sys->p_header->frames_per_packet )
    {
        block_t *p_aout_buffer;
        if( p_sys->p_header->frame_size == 0 )
            return NULL;
        p_aout_buffer =
            decoder_NewAudioBuffer( p_dec, p_sys->p_header->frame_size );
        if( !p_aout_buffer )
        {
            return NULL;
        }
        switch( speex_decode_int( p_sys->p_state, &p_sys->bits,
                                  (int16_t *)p_aout_buffer->p_buffer ) )
        {
            case -2:
                msg_Err( p_dec, "decoding error: corrupted stream?" );
            case -1: /* End of stream */
                return NULL;
        }
        if( speex_bits_remaining( &p_sys->bits ) < 0 )
        {
            msg_Err( p_dec, "decoding overflow: corrupted stream?" );
        }
        if( p_sys->p_header->nb_channels == 2 )
            speex_decode_stereo_int( (int16_t *)p_aout_buffer->p_buffer,
                                     p_sys->p_header->frame_size,
                                     &p_sys->stereo );
        /* Date management */
        p_aout_buffer->i_pts = date_Get( &p_sys->end_date );
        p_aout_buffer->i_length =
            date_Increment( &p_sys->end_date, p_sys->p_header->frame_size )
            - p_aout_buffer->i_pts;
        p_sys->i_frame_in_packet++;
        return p_aout_buffer;
    }
    else
    {
        return NULL;
    }
}
