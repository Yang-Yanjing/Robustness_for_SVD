/*** Initialization ***/
static int Open (vlc_object_t *obj)
{
    filter_t *filter = (filter_t *)obj;
    if (filter->fmt_in.video.orientation != filter->fmt_out.video.orientation)
        return VLC_EGENERIC;
    if ((filter->fmt_in.video.i_width != filter->fmt_out.video.i_width)
     || (filter->fmt_in.video.i_height != filter->fmt_out.video.i_height))
        return OpenScaler (obj);
    switch (filter->fmt_in.video.i_chroma)
    {
        case VLC_CODEC_I420:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_RGB16:
                    if (FixRV16 (&filter->fmt_out.video))
                        return VLC_EGENERIC;
                    filter->pf_video_filter = I420_RV16_Filter;
                    return VLC_SUCCESS;
            }
            break;
        case VLC_CODEC_YV12:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_RGB16:
                    if (FixRV16 (&filter->fmt_out.video))
                        return VLC_EGENERIC;
                    filter->pf_video_filter = YV12_RV16_Filter;
                    return VLC_SUCCESS;
            }
            break;
        case VLC_CODEC_I422:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I420:
                    filter->pf_video_filter = I422_I420_Filter;
                    return VLC_SUCCESS;
                case VLC_CODEC_YV12:
                    filter->pf_video_filter = I422_YV12_Filter;
                    return VLC_SUCCESS;
            }
            break;
        case VLC_CODEC_I444:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_RGB16:
                    if (FixRV16 (&filter->fmt_out.video))
                        return VLC_EGENERIC;
                    filter->pf_video_filter = I444_RV16_Filter;
                    return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        case VLC_CODEC_YUYV:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_RGB24:
                    if (FixRV24 (&filter->fmt_out.video))
                        return VLC_EGENERIC;
                    filter->pf_video_filter = YUYV_RV24_Filter;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB16:
                    if (FixRV16 (&filter->fmt_out.video))
                        return VLC_EGENERIC;
                    filter->pf_video_filter = YUYV_RV16_Filter;
                    return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        case VLC_CODEC_UYVY:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I420:
                    filter->pf_video_filter = UYVY_I420_Filter;
                    return VLC_SUCCESS;
                case VLC_CODEC_YV12:
                    filter->pf_video_filter = UYVY_YV12_Filter;
                    return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
    }
    /* fallback to scaler (conversion with 1:1 scale) */
    return OpenScaler (obj);
}
