 *****************************************************************************/
static int OpenEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t*)p_this;
    int status;
    if( !GetOmxRole(p_enc->fmt_out.i_codec, p_enc->fmt_in.i_cat, true) )
        return VLC_EGENERIC;
    status = OpenGeneric( p_this, true );
    if(status != VLC_SUCCESS) return status;
    p_enc->pf_encode_video = EncodeVideo;
    return VLC_SUCCESS;
}
