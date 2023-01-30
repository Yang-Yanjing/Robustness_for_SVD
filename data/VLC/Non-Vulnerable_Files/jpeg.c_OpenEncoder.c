 */
static int OpenEncoder(vlc_object_t *p_this)
{
    encoder_t *p_enc = (encoder_t *)p_this;
    config_ChainParse(p_enc, ENC_CFG_PREFIX, ppsz_enc_options, p_enc->p_cfg);
    if (p_enc->fmt_out.i_codec != VLC_CODEC_JPEG)
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store encoder's structure */
    encoder_sys_t *p_sys = malloc(sizeof(encoder_sys_t));
    if (p_sys == NULL)
    {
        return VLC_ENOMEM;
    }
    p_enc->p_sys = p_sys;
    p_sys->p_obj = p_this;
    p_sys->p_jpeg.err = jpeg_std_error(&p_sys->err);
    p_sys->err.error_exit = user_error_exit;
    p_sys->err.output_message = user_error_message;
    p_sys->i_quality = var_GetInteger(p_enc, ENC_CFG_PREFIX "quality");
    p_sys->i_blocksize = 3 * p_enc->fmt_in.video.i_visible_width * p_enc->fmt_in.video.i_visible_height;
    p_enc->fmt_in.i_codec = VLC_CODEC_J420;
    p_enc->pf_encode_video = EncodeBlock;
    return VLC_SUCCESS;
}
