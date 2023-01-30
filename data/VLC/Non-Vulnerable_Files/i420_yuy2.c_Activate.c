 *****************************************************************************/
static int Activate( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    if( !vlc_CPU_capable() )
        return VLC_EGENERIC;
    if( (p_filter->fmt_in.video.i_x_offset + p_filter->fmt_in.video.i_visible_width) & 1
     || (p_filter->fmt_in.video.i_y_offset + p_filter->fmt_in.video.i_visible_height) & 1 )
    {
        return -1;
    }
    if( p_filter->fmt_in.video.i_width != p_filter->fmt_out.video.i_width
       || p_filter->fmt_in.video.i_height != p_filter->fmt_out.video.i_height
       || p_filter->fmt_in.video.orientation != p_filter->fmt_out.video.orientation )
        return -1;
    switch( p_filter->fmt_in.video.i_chroma )
    {
//        case VLC_CODEC_YV12: FIXME invert U and V in the filters :)
        case VLC_CODEC_I420:
            switch( p_filter->fmt_out.video.i_chroma )
            {
                case VLC_CODEC_YUYV:
                    p_filter->pf_video_filter = I420_YUY2_Filter;
                    break;
                case VLC_CODEC_YVYU:
                    p_filter->pf_video_filter = I420_YVYU_Filter;
                    break;
                case VLC_CODEC_UYVY:
                    p_filter->pf_video_filter = I420_UYVY_Filter;
                    break;
#if !defined (MODULE_NAME_IS_i420_yuy2_altivec)
                case VLC_FOURCC('I','U','Y','V'):
                    p_filter->pf_video_filter = I420_IUYV_Filter;
                    break;
                case VLC_CODEC_CYUV:
                    p_filter->pf_video_filter = I420_cyuv_Filter;
                    break;
#endif
#if defined (MODULE_NAME_IS_i420_yuy2)
                case VLC_CODEC_Y211:
                    p_filter->pf_video_filter = I420_Y211_Filter;
                    break;
#endif
                default:
                    return -1;
            }
            break;
        default:
            return -1;
    }
    return 0;
}
