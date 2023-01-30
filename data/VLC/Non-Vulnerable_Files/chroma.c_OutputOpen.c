}
static int OutputOpen(vlc_object_t *obj)
{
    filter_t *filter = (filter_t *)obj;
    if (filter->fmt_out.video.i_chroma != VLC_CODEC_VDPAU_OUTPUT)
        return VLC_EGENERIC;
    assert(filter->fmt_out.video.orientation == ORIENT_TOP_LEFT);
    filter_sys_t *sys = malloc(sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    sys->vdp = NULL;
    sys->mixer = VDP_INVALID_HANDLE;
    if (filter->fmt_in.video.i_chroma == VLC_CODEC_VDPAU_VIDEO_444)
    {
        sys->chroma = VDP_CHROMA_TYPE_444;
        sys->format = VDP_YCBCR_FORMAT_NV12;
        filter->pf_video_filter = VideoRender;
    }
    else
    if (filter->fmt_in.video.i_chroma == VLC_CODEC_VDPAU_VIDEO_422)
    {
        sys->chroma = VDP_CHROMA_TYPE_422;
        /* TODO: check if the drivery supports NV12 or UYVY */
        sys->format = VDP_YCBCR_FORMAT_UYVY;
        filter->pf_video_filter = VideoRender;
    }
    else
    if (filter->fmt_in.video.i_chroma == VLC_CODEC_VDPAU_VIDEO_420)
    {
        sys->chroma = VDP_CHROMA_TYPE_420;
        sys->format = VDP_YCBCR_FORMAT_NV12;
        filter->pf_video_filter = VideoRender;
    }
    else
    if (vlc_fourcc_to_vdp_ycc(filter->fmt_in.video.i_chroma,
                              &sys->chroma, &sys->format))
        filter->pf_video_filter = YCbCrRender;
    else
    {
        free(sys);
        return VLC_EGENERIC;
    }
    /* NOTE: The video mixer capabilities should be checked here, and the
     * then video mixer set up. But:
     * 1) The VDPAU back-end is accessible only once the first picture
     *    gets filtered. Thus the video mixer is created later.
     * 2) Bailing out due to insufficient capabilities would break the
     *    video pipeline. Thus capabilities should be checked earlier. */
    for (unsigned i = 0; i < MAX_PAST + MAX_FUTURE; i++)
        sys->history[i].field = NULL;
    sys->procamp.brightness = 0.f;
    sys->procamp.contrast = 1.f;
    sys->procamp.saturation = 1.f;
    sys->procamp.hue = 0.f;
    filter->pf_video_flush = Flush;
    filter->p_sys = sys;
    return VLC_SUCCESS;
}
