VIDEO_FILTER_WRAPPER (VYUY_I422)
static int Open (vlc_object_t *obj)
{
    filter_t *filter = (filter_t *)obj;
    if (!vlc_CPU_ARM_NEON())
        return VLC_EGENERIC;
    if ((filter->fmt_in.video.i_width != filter->fmt_out.video.i_width)
     || (filter->fmt_in.video.i_height != filter->fmt_out.video.i_height))
        return VLC_EGENERIC;
    switch (filter->fmt_in.video.i_chroma)
    {
        /* Planar to packed */
        case VLC_CODEC_I420:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_YUYV:
                    filter->pf_video_filter = I420_YUYV_Filter;
                    break;
                case VLC_CODEC_UYVY:
                    filter->pf_video_filter = I420_UYVY_Filter;
                    break;
                case VLC_CODEC_YVYU:
                    filter->pf_video_filter = I420_YVYU_Filter;
                    break;
                case VLC_CODEC_VYUY:
                    filter->pf_video_filter = I420_VYUY_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
            break;
        case VLC_CODEC_YV12:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_YUYV:
                    filter->pf_video_filter = I420_YVYU_Filter;
                    break;
                case VLC_CODEC_UYVY:
                    filter->pf_video_filter = I420_VYUY_Filter;
                    break;
                case VLC_CODEC_YVYU:
                    filter->pf_video_filter = I420_YUYV_Filter;
                    break;
                case VLC_CODEC_VYUY:
                    filter->pf_video_filter = I420_UYVY_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
            break;
        case VLC_CODEC_I422:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_YUYV:
                    filter->pf_video_filter = I422_YUYV_Filter;
                    break;
                case VLC_CODEC_UYVY:
                    filter->pf_video_filter = I422_UYVY_Filter;
                    break;
                case VLC_CODEC_YVYU:
                    filter->pf_video_filter = I422_YVYU_Filter;
                    break;
                case VLC_CODEC_VYUY:
                    filter->pf_video_filter = I422_VYUY_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
            break;
        /* Semiplanar to planar */
        case VLC_CODEC_NV12:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I420:
                    filter->pf_video_filter = Semiplanar_Planar_420_Filter;
                    break;
                case VLC_CODEC_YV12:
                    filter->pf_video_filter = Semiplanar_Planar_420_Swap_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
            break;
        case VLC_CODEC_NV21:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I420:
                    filter->pf_video_filter = Semiplanar_Planar_420_Swap_Filter;
                    break;
                case VLC_CODEC_YV12:
                    filter->pf_video_filter = Semiplanar_Planar_420_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
            break;
        case VLC_CODEC_NV16:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I422:
                    filter->pf_video_filter = Semiplanar_Planar_422_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
            break;
        case VLC_CODEC_NV24:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I444:
                    filter->pf_video_filter = Semiplanar_Planar_444_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
            break;
        /* Packed to planar */
        case VLC_CODEC_YUYV:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I422:
                    filter->pf_video_filter = YUYV_I422_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
        case VLC_CODEC_UYVY:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I422:
                    filter->pf_video_filter = UYVY_I422_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
        case VLC_CODEC_YVYU:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I422:
                    filter->pf_video_filter = YVYU_I422_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
        case VLC_CODEC_VYUY:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I422:
                    filter->pf_video_filter = VYUY_I422_Filter;
                    break;
                default:
                    return VLC_EGENERIC;
            }
        default:
            return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
