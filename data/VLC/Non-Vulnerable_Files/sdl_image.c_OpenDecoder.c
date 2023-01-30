 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys;
    int i;
    /* Find codec. */
    for ( i = 0;
          i < (int)(sizeof(p_supported_fmt)/sizeof(struct supported_fmt_t));
          i++ )
    {
        if ( p_supported_fmt[i].i_fourcc == p_dec->fmt_in.i_codec )
            break;
    }
    if ( i == (int)(sizeof(p_supported_fmt)/sizeof(struct supported_fmt_t)) )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys =
          (decoder_sys_t *)malloc(sizeof(decoder_sys_t)) ) == NULL )
        return VLC_ENOMEM;
    p_sys->psz_sdl_type = p_supported_fmt[i].psz_sdl_type;
    /* Set output properties - this is a decoy and isn't used anywhere */
    p_dec->fmt_out.i_cat = VIDEO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_RGB32;
    /* Set callbacks */
    p_dec->pf_decode_video = DecodeBlock;
    return VLC_SUCCESS;
}
