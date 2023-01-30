 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    vlc_fourcc_t fourcc = p_filter->fmt_in.video.i_chroma;
    if( fourcc == VLC_CODEC_YUVP || fourcc == VLC_CODEC_RGBP
     || fourcc == VLC_CODEC_RGBA || fourcc == VLC_CODEC_ARGB )
        return VLC_EGENERIC;
    const vlc_chroma_description_t *p_chroma =
        vlc_fourcc_GetChromaDescription( fourcc );
    if( p_chroma == NULL || p_chroma->plane_count == 0
     || p_chroma->pixel_size * 8 != p_chroma->pixel_bits )
        return VLC_EGENERIC;
    p_filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
