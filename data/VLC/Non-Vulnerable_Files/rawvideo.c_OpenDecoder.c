}
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    int ret = OpenCommon( p_dec );
    if( ret == VLC_SUCCESS )
        p_dec->pf_decode_video = DecodeFrame;
    return ret;
}
