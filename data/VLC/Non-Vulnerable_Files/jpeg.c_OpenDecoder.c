 */
static int OpenDecoder(vlc_object_t *p_this)
{
    decoder_t *p_dec = (decoder_t *)p_this;
    if (p_dec->fmt_in.i_codec != VLC_CODEC_JPEG)
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    decoder_sys_t *p_sys = malloc(sizeof(decoder_sys_t));
    if (p_sys == NULL)
    {
        return VLC_ENOMEM;
    }
    p_dec->p_sys = p_sys;
    p_sys->p_obj = p_this;
    p_sys->p_jpeg.err = jpeg_std_error(&p_sys->err);
    p_sys->err.error_exit = user_error_exit;
    p_sys->err.output_message = user_error_message;
    /* Set output properties */
    p_dec->fmt_out.i_cat = VIDEO_ES;
    /* Set callbacks */
    p_dec->pf_decode_video = DecodeBlock;
    return VLC_SUCCESS;
}
