 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_SPEEX )
        return VLC_EGENERIC;
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys = malloc(sizeof(decoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    p_dec->p_sys->bits.buf_size = 0;
    p_dec->p_sys->b_packetizer = false;
    p_dec->p_sys->rtp_rate = p_dec->fmt_in.audio.i_rate;
    p_dec->p_sys->b_has_headers = false;
    date_Set( &p_sys->end_date, 0 );
    /* Set output properties */
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_S16N;
    /*
      Set callbacks
      If the codec is spxr then this decoder is
      being invoked on a Speex stream arriving via RTP.
      A special decoder callback is used.
    */
    if (p_dec->fmt_in.i_original_fourcc == VLC_FOURCC('s', 'p', 'x', 'r'))
    {
        msg_Dbg( p_dec, "Using RTP version of Speex decoder @ rate %d.",
        p_dec->fmt_in.audio.i_rate );
        p_dec->pf_decode_audio = DecodeRtpSpeexPacket;
    }
    else
    {
        p_dec->pf_decode_audio = DecodeBlock;
    }
    p_dec->pf_packetize    = DecodeBlock;
    p_sys->p_state = NULL;
    p_sys->p_header = NULL;
    p_sys->i_frame_in_packet = 0;
    return VLC_SUCCESS;
}
