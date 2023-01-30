 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_MLP &&
        p_dec->fmt_in.i_codec != VLC_CODEC_TRUEHD )
        return VLC_EGENERIC;
    /* */
    p_dec->p_sys = p_sys = malloc( sizeof(*p_sys) );
    if( !p_sys )
        return VLC_ENOMEM;
    /* */
    p_sys->i_state = STATE_NOSYNC;
    date_Set( &p_sys->end_date, 0 );
    block_BytestreamInit( &p_sys->bytestream );
    p_sys->b_mlp = false;
    /* Set output properties */
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.i_codec = p_dec->fmt_in.i_codec;
    p_dec->fmt_out.audio.i_rate = 0;
    /* Set callback */
    p_dec->pf_packetize = Packetize;
    return VLC_SUCCESS;
}
