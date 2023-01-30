 *****************************************************************************/
static int OpenEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    p_enc->pf_encode_video = EncodeVideo;
    p_enc->pf_encode_audio = EncodeAudio;
    return VLC_SUCCESS;
}
