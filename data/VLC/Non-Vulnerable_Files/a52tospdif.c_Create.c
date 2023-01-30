 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t * p_filter = (filter_t *)p_this;
    if ( p_filter->fmt_in.audio.i_format != VLC_CODEC_A52 ||
         ( p_filter->fmt_out.audio.i_format != VLC_CODEC_SPDIFB &&
           p_filter->fmt_out.audio.i_format != VLC_CODEC_SPDIFL ) )
    {
        return VLC_EGENERIC;
    }
    p_filter->pf_audio_filter = DoWork;
    return VLC_SUCCESS;
}
