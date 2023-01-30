 *****************************************************************************/
static int OpenFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    if( ( p_filter->fmt_in.video.i_chroma != VLC_CODEC_YUVP &&
          p_filter->fmt_in.video.i_chroma != VLC_CODEC_YUVA &&
          p_filter->fmt_in.video.i_chroma != VLC_CODEC_I420 &&
          p_filter->fmt_in.video.i_chroma != VLC_CODEC_YV12 &&
          p_filter->fmt_in.video.i_chroma != VLC_CODEC_RGB32 &&
          p_filter->fmt_in.video.i_chroma != VLC_CODEC_RGBA &&
          p_filter->fmt_in.video.i_chroma != VLC_CODEC_ARGB ) ||
        p_filter->fmt_in.video.i_chroma != p_filter->fmt_out.video.i_chroma )
    {
        return VLC_EGENERIC;
    }
    if( p_filter->fmt_in.video.orientation != p_filter->fmt_out.video.orientation )
        return VLC_EGENERIC;
    video_format_ScaleCropAr( &p_filter->fmt_out.video, &p_filter->fmt_in.video );
    p_filter->pf_video_filter = Filter;
    msg_Dbg( p_filter, "%ix%i -> %ix%i", p_filter->fmt_in.video.i_width,
             p_filter->fmt_in.video.i_height, p_filter->fmt_out.video.i_width,
             p_filter->fmt_out.video.i_height );
    return VLC_SUCCESS;
}
