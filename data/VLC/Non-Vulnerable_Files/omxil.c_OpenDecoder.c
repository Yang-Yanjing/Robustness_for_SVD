 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    int status;
#ifdef __ANDROID__
    if( p_dec->fmt_in.i_cat == AUDIO_ES )
        return VLC_EGENERIC;
#endif
    if( 0 || !GetOmxRole(p_dec->fmt_in.i_codec, p_dec->fmt_in.i_cat, false) )
        return VLC_EGENERIC;
    status = OpenGeneric( p_this, false );
    if(status != VLC_SUCCESS) return status;
    p_dec->pf_decode_video = DecodeVideo;
    p_dec->pf_decode_audio = DecodeAudio;
    return VLC_SUCCESS;
}
