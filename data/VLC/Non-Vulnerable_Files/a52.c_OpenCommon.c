 *****************************************************************************/
static int OpenCommon( vlc_object_t *p_this, bool b_packetizer )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    vlc_fourcc_t i_codec;
    switch( p_dec->fmt_in.i_codec )
    {
    case VLC_CODEC_A52:
        i_codec = VLC_CODEC_A52;
        break;
    case VLC_CODEC_EAC3:
        /* XXX ugly hack, a52 does not support eac3 so no eac3 pass-through
         * support */
        if( !b_packetizer )
            return VLC_EGENERIC;
        i_codec = VLC_CODEC_EAC3;
        break;
    default:
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys =
          (decoder_sys_t *)malloc(sizeof(decoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    /* Misc init */
    p_sys->b_packetizer = b_packetizer;
    p_sys->i_state = STATE_NOSYNC;
    date_Set( &p_sys->end_date, 0 );
    p_sys->i_pts = VLC_TS_INVALID;
    block_BytestreamInit( &p_sys->bytestream );
    /* Set output properties */
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.i_codec = i_codec;
    p_dec->fmt_out.audio.i_rate = 0; /* So end_date gets initialized */
    p_dec->fmt_out.audio.i_bytes_per_frame = 0;
    /* Set callback */
    if( b_packetizer )
        p_dec->pf_packetize    = DecodeBlock;
    else
        p_dec->pf_decode_audio = DecodeBlock;
    return VLC_SUCCESS;
}
