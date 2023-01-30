 *****************************************************************************/
static int OpenCommon( vlc_object_t *p_this, bool b_packetizer )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_DTS )
        return VLC_EGENERIC;
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys = malloc(sizeof(*p_sys)) ) == NULL )
        return VLC_ENOMEM;
    /* Misc init */
    p_sys->b_packetizer = b_packetizer;
    p_sys->i_state = STATE_NOSYNC;
    date_Set( &p_sys->end_date, 0 );
    p_sys->b_dts_hd = false;
    p_sys->i_pts = VLC_TS_INVALID;
    block_BytestreamInit( &p_sys->bytestream );
    /* Set output properties */
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_DTS;
    p_dec->fmt_out.audio.i_rate = 0; /* So end_date gets initialized */
    /* Set callback */
    p_dec->pf_decode_audio = DecodeBlock;
    p_dec->pf_packetize    = DecodeBlock;
    return VLC_SUCCESS;
}
