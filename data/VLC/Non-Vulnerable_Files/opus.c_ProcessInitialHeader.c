 *****************************************************************************/
static int ProcessInitialHeader( decoder_t *p_dec, ogg_packet *p_oggpacket )
{
    int err;
    unsigned char new_stream_map[8];
    decoder_sys_t *p_sys = p_dec->p_sys;
    OpusHeader *p_header = &p_sys->header;
    if( !opus_header_parse((unsigned char *)p_oggpacket->packet,p_oggpacket->bytes,p_header) )
    {
        msg_Err( p_dec, "cannot read Opus header" );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_dec, "Opus audio with %d channels", p_header->channels);
    if((p_header->channels>2 && p_header->channel_mapping==0) ||
        p_header->channels>8 ||
        p_header->channel_mapping>1)
    {
        msg_Err( p_dec, "Unsupported channel mapping" );
        return VLC_EGENERIC;
    }
    /* Setup the format */
    p_dec->fmt_out.audio.i_physical_channels =
        p_dec->fmt_out.audio.i_original_channels =
            pi_channels_maps[p_header->channels];
    p_dec->fmt_out.audio.i_channels = p_header->channels;
    p_dec->fmt_out.audio.i_rate = 48000;
    if( p_header->channels>2 )
    {
        static const uint32_t *pi_ch[6] = { pi_3channels_in, pi_4channels_in,
                                            pi_5channels_in, pi_6channels_in,
                                            pi_7channels_in, pi_8channels_in };
        uint8_t pi_chan_table[AOUT_CHAN_MAX];
        aout_CheckChannelReorder( pi_ch[p_header->channels-3], NULL,
                                  p_dec->fmt_out.audio.i_physical_channels,
                                  pi_chan_table );
        for(int i=0;i<p_header->channels;i++)
            new_stream_map[pi_chan_table[i]]=p_header->stream_map[i];
    }
    /* Opus decoder init */
    p_sys->p_st = opus_multistream_decoder_create( 48000, p_header->channels,
                    p_header->nb_streams, p_header->nb_coupled,
                    p_header->channels>2?new_stream_map:p_header->stream_map,
                    &err );
    if( !p_sys->p_st || err!=OPUS_OK )
    {
        msg_Err( p_dec, "decoder initialization failed" );
        return VLC_EGENERIC;
    }
#ifdef OPUS_SET_GAIN
    if( opus_multistream_decoder_ctl( p_sys->p_st,OPUS_SET_GAIN(p_header->gain) ) != OPUS_OK )
    {
        msg_Err( p_dec, "OPUS_SET_GAIN failed" );
        opus_multistream_decoder_destroy( p_sys->p_st );
        return VLC_EGENERIC;
    }
#endif
    date_Init( &p_sys->end_date, 48000, 1 );
    return VLC_SUCCESS;
}
