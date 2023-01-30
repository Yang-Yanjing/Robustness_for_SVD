}
const vlc_fourcc_t *vlc_fourcc_GetYUVFallback( vlc_fourcc_t i_fourcc )
{
    return GetFallback( i_fourcc, pp_YUV_fallback, p_list_YUV );
}
