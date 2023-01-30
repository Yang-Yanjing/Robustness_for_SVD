}
static int OpenEncoder(vlc_object_t *p_this)
{
    encoder_t *p_enc = (encoder_t *) p_this;
    if( p_enc->fmt_out.i_codec != VLC_CODEC_PNG )
        return VLC_EGENERIC;
    /* Allocate the memory needed to store the encoder's structure */
    p_enc->p_sys = malloc( sizeof(encoder_sys_t) );
    if( p_enc->p_sys  == NULL )
        return VLC_ENOMEM;
    p_enc->p_sys->p_obj = p_this;
    p_enc->p_sys->i_blocksize = 3 * p_enc->fmt_in.video.i_visible_width *
        p_enc->fmt_in.video.i_visible_height;
    p_enc->fmt_in.i_codec = VLC_CODEC_RGB24;
    p_enc->fmt_in.video.i_rmask = 0x000000ff;
    p_enc->fmt_in.video.i_gmask = 0x0000ff00;
    p_enc->fmt_in.video.i_bmask = 0x00ff0000;
    p_enc->pf_encode_video = EncodeBlock;
    return VLC_SUCCESS;
}
