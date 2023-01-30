}
static int YCbCrOpen(vlc_object_t *obj)
{
    filter_t *filter = (filter_t *)obj;
    if (filter->fmt_in.video.i_chroma != VLC_CODEC_VDPAU_VIDEO_420
     && filter->fmt_in.video.i_chroma != VLC_CODEC_VDPAU_VIDEO_422
     && filter->fmt_in.video.i_chroma != VLC_CODEC_VDPAU_VIDEO_444)
        return VLC_EGENERIC;
    if (filter->fmt_in.video.i_visible_width
                                       != filter->fmt_out.video.i_visible_width
     || filter->fmt_in.video.i_visible_height
                                      != filter->fmt_out.video.i_visible_height
     || filter->fmt_in.video.i_x_offset != filter->fmt_out.video.i_x_offset
     || filter->fmt_in.video.i_y_offset != filter->fmt_out.video.i_y_offset
     || (filter->fmt_in.video.i_sar_num * filter->fmt_out.video.i_sar_den
          != filter->fmt_in.video.i_sar_den * filter->fmt_out.video.i_sar_num))
        return VLC_EGENERIC;
    filter_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    if (!vlc_fourcc_to_vdp_ycc(filter->fmt_out.video.i_chroma,
                               &sys->chroma, &sys->format))
    {
        free(sys);
        return VLC_EGENERIC;
    }
    filter->pf_video_filter = VideoExport_Filter;
    filter->p_sys = sys;
    return VLC_SUCCESS;
}
