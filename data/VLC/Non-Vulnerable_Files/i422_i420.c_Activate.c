 *****************************************************************************/
static int Activate( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    if( p_filter->fmt_in.video.i_width & 1
     || p_filter->fmt_in.video.i_height & 1 )
    {
        return -1;
    }
    if( p_filter->fmt_in.video.i_width != p_filter->fmt_out.video.i_width
       || p_filter->fmt_in.video.i_height != p_filter->fmt_out.video.i_height
       || p_filter->fmt_in.video.orientation != p_filter->fmt_out.video.orientation )
        return -1;
    switch( p_filter->fmt_in.video.i_chroma )
    {
        case VLC_CODEC_I422:
        case VLC_CODEC_J422:
            switch( p_filter->fmt_out.video.i_chroma )
            {
                case VLC_CODEC_I420:
                case VLC_CODEC_J420:
                    p_filter->pf_video_filter = I422_I420_Filter;
                    break;
                case VLC_CODEC_YV12:
                    p_filter->pf_video_filter = I422_YV12_Filter;
                    break;
                case VLC_CODEC_YUV420A:
                    p_filter->pf_video_filter = I422_YUVA_Filter;
                    break;
                default:
                    return -1;
            }
            break;
        default:
            return -1;
    }
    return 0;
}
