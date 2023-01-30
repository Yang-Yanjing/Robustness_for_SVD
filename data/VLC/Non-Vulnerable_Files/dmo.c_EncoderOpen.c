 *****************************************************************************/
static int EncoderOpen( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t*)p_this;
    int i_ret = EncOpen( p_this );
    if( i_ret != VLC_SUCCESS ) return i_ret;
    /* Set callbacks */
    p_enc->pf_encode_video = (block_t *(*)(encoder_t *, picture_t *))
        EncodeBlock;
    p_enc->pf_encode_audio = (block_t *(*)(encoder_t *, block_t *))
        EncodeBlock;
    return VLC_SUCCESS;
}
