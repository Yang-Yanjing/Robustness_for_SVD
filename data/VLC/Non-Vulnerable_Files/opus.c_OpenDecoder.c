 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_OPUS )
        return VLC_EGENERIC;
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys = malloc(sizeof(decoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    p_dec->p_sys->b_has_headers = false;
    date_Set( &p_sys->end_date, 0 );
    /* Set output properties */
    p_dec->fmt_out.i_cat = AUDIO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_FL32;
    p_dec->pf_decode_audio = DecodeBlock;
    p_dec->pf_packetize    = DecodeBlock;
    p_sys->p_st = NULL;
    return VLC_SUCCESS;
}
