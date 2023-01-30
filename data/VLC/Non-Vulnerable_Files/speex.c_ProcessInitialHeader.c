 *****************************************************************************/
static int ProcessInitialHeader( decoder_t *p_dec, ogg_packet *p_oggpacket )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    void *p_state;
    SpeexHeader *p_header;
    const SpeexMode *p_mode;
    SpeexCallback callback;
    p_sys->p_header = p_header =
        speex_packet_to_header( (char *)p_oggpacket->packet,
                                p_oggpacket->bytes );
    if( !p_header )
    {
        msg_Err( p_dec, "cannot read Speex header" );
        return VLC_EGENERIC;
    }
    if( p_header->mode >= SPEEX_NB_MODES || p_header->mode < 0 )
    {
        msg_Err( p_dec, "mode number %d does not (yet/any longer) exist in "
                 "this version of libspeex.", p_header->mode );
        return VLC_EGENERIC;
    }
    p_mode = speex_mode_list[p_header->mode];
    if( p_mode == NULL )
        return VLC_EGENERIC;
    if( p_header->speex_version_id > 1 )
    {
        msg_Err( p_dec, "this file was encoded with Speex bit-stream "
                 "version %d which is not supported by this decoder.",
                 p_header->speex_version_id );
        return VLC_EGENERIC;
    }
    if( p_mode->bitstream_version < p_header->mode_bitstream_version )
    {
        msg_Err( p_dec, "file encoded with a newer version of Speex." );
        return VLC_EGENERIC;
    }
    if( p_mode->bitstream_version > p_header->mode_bitstream_version )
    {
        msg_Err( p_dec, "file encoded with an older version of Speex." );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_dec, "Speex %d Hz audio using %s mode %s%s",
             p_header->rate, p_mode->modeName,
             ( p_header->nb_channels == 1 ) ? " (mono" : " (stereo",
             p_header->vbr ? ", VBR)" : ")" );
    /* Take care of speex decoder init */
    speex_bits_init( &p_sys->bits );
    p_sys->p_state = p_state = speex_decoder_init( p_mode );
    if( !p_state )
    {
        msg_Err( p_dec, "decoder initialization failed" );
        return VLC_EGENERIC;
    }
    if( p_header->nb_channels == 2 )
    {
        SpeexStereoState stereo = SPEEX_STEREO_STATE_INIT;
        p_sys->stereo = stereo;
        callback.callback_id = SPEEX_INBAND_STEREO;
        callback.func = speex_std_stereo_request_handler;
        callback.data = &p_sys->stereo;
        speex_decoder_ctl( p_state, SPEEX_SET_HANDLER, &callback );
    }
    if( p_header->nb_channels <= 0 ||
        p_header->nb_channels > 5 )
    {
        msg_Err( p_dec, "invalid number of channels (not between 1 and 5): %i",
                 p_header->nb_channels );
        return VLC_EGENERIC;
    }
    /* Setup the format */
    p_dec->fmt_out.audio.i_physical_channels =
        p_dec->fmt_out.audio.i_original_channels =
            pi_channels_maps[p_header->nb_channels];
    p_dec->fmt_out.audio.i_channels = p_header->nb_channels;
    p_dec->fmt_out.audio.i_rate = p_header->rate;
    date_Init( &p_sys->end_date, p_header->rate, 1 );
    return VLC_SUCCESS;
}
