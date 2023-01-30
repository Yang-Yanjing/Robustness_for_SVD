 *****************************************************************************/
static int OpenFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    /* XXX Only support RV24 -> RV32 conversion */
    if( p_filter->fmt_in.video.i_chroma != VLC_CODEC_RGB24 ||
        (p_filter->fmt_out.video.i_chroma != VLC_CODEC_RGB32 &&
        p_filter->fmt_out.video.i_chroma != VLC_CODEC_RGBA) )
    {
        return VLC_EGENERIC;
    }
    if( p_filter->fmt_in.video.i_width != p_filter->fmt_out.video.i_width
     || p_filter->fmt_in.video.i_height != p_filter->fmt_out.video.i_height
     || p_filter->fmt_in.video.orientation != p_filter->fmt_out.video.orientation)
        return -1;
    p_filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
