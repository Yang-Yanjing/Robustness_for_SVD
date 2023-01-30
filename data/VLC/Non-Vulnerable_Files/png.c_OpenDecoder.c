 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_PNG &&
        p_dec->fmt_in.i_codec != VLC_FOURCC('M','P','N','G') )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    p_dec->p_sys = malloc( sizeof(decoder_sys_t) );
    if( p_dec->p_sys == NULL )
        return VLC_ENOMEM;
    p_dec->p_sys->p_obj = p_this;
    /* Set output properties */
    p_dec->fmt_out.i_cat = VIDEO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_RGBA;
    /* Set callbacks */
    p_dec->pf_decode_video = DecodeBlock;
    return VLC_SUCCESS;
}
