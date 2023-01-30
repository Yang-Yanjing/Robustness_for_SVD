/*** Scaling initialization ***/
static int OpenScaler (vlc_object_t *obj)
{
    filter_t *filter = (filter_t *)obj;
    switch (filter->fmt_in.video.i_chroma)
    {
        case VLC_CODEC_I420:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I420:
                    filter->pf_video_filter = I420_I420_Scale_Filter;
                    return VLC_SUCCESS;
                case VLC_CODEC_YV12:
                    filter->pf_video_filter = I420_YV12_Scale_Filter;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB16:
                    if (FixRV16 (&filter->fmt_out.video))
                        return VLC_EGENERIC;
                    filter->pf_video_filter = I420_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR565;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB15:
                    if (FixRV15 (&filter->fmt_out.video))
                        break;
                    filter->pf_video_filter = I420_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR555;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB12:
                    if (FixRV12 (&filter->fmt_out.video))
                        break;
                    filter->pf_video_filter = I420_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR444;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB24:
                    if (FixRV24 (&filter->fmt_out.video))
                        break;
                    filter->pf_video_filter = I420_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR888;
                    return VLC_SUCCESS;
            }
            break;
        case VLC_CODEC_YV12:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I420:
                    filter->pf_video_filter = YV12_I420_Scale_Filter;
                    return VLC_SUCCESS;
                case VLC_CODEC_YV12:
                    filter->pf_video_filter = YV12_YV12_Scale_Filter;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB16:
                    if (FixRV16 (&filter->fmt_out.video))
                        break;
                    filter->pf_video_filter = YV12_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR565;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB15:
                    if (FixRV15 (&filter->fmt_out.video))
                        break;
                    filter->pf_video_filter = YV12_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR555;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB12:
                    if (FixRV12 (&filter->fmt_out.video))
                        break;
                    filter->pf_video_filter = YV12_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR444;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB24:
                    if (FixRV24 (&filter->fmt_out.video))
                        break;
                    filter->pf_video_filter = YV12_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR888;
                    return VLC_SUCCESS;
            }
            break;
        case VLC_CODEC_I422:
            switch (filter->fmt_out.video.i_chroma)
            {
                case VLC_CODEC_I422:
                    filter->pf_video_filter = I422_I422_Scale_Filter;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB16:
                    if (FixRV16 (&filter->fmt_out.video))
                        break;
                    filter->pf_video_filter = I422_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR565;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB15:
                    if (FixRV15 (&filter->fmt_out.video))
                        break;
                    filter->pf_video_filter = I422_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR555;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB12:
                    if (FixRV12 (&filter->fmt_out.video))
                        break;
                    filter->pf_video_filter = I422_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR444;
                    return VLC_SUCCESS;
                case VLC_CODEC_RGB24:
                    if (FixRV24 (&filter->fmt_out.video))
                        break;
                    filter->pf_video_filter = I422_RGB_Scale_Filter;
                    filter->p_sys = (void *)(intptr_t)OMX_IP_BGR888;
                    return VLC_SUCCESS;
            }
            break;
    }
    return VLC_EGENERIC;
}
