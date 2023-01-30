/** Import VLC picture into VDPAU video surface */
static picture_t *VideoImport(filter_t *filter, picture_t *src)
{
    filter_sys_t *sys = filter->p_sys;
    VdpVideoSurface surface;
    VdpStatus err;
    if (sys->vdp == NULL)
        goto drop;
    /* Create surface (TODO: reuse?) */
    err = vdp_video_surface_create(sys->vdp, sys->device, sys->chroma,
                                   filter->fmt_in.video.i_width,
                                   filter->fmt_in.video.i_visible_height,
                                   &surface);
    if (err != VDP_STATUS_OK)
    {
        msg_Err(filter, "video %s %s failure: %s", "surface", "creation",
                vdp_get_error_string(sys->vdp, err));
        goto drop;
    }
    /* Put bits */
    const void *planes[3];
    uint32_t pitches[3];
    for (int i = 0; i < src->i_planes; i++)
    {
        planes[i] = src->p[i].p_pixels
                  + filter->fmt_in.video.i_y_offset * src->p[i].i_pitch;
        pitches[i] = src->p[i].i_pitch;
    }
    if (src->format.i_chroma == VLC_CODEC_I420)
    {
        planes[1] = src->p[2].p_pixels;
        planes[2] = src->p[1].p_pixels;
        pitches[1] = src->p[2].i_pitch;
        pitches[2] = src->p[1].i_pitch;
    }
    if (src->format.i_chroma == VLC_CODEC_I420
     || src->format.i_chroma == VLC_CODEC_YV12
     || src->format.i_chroma == VLC_CODEC_NV12)
    {
        for (int i = 1; i < src->i_planes; i++)
            planes[i] = ((const uint8_t *)planes[i])
                + (filter->fmt_in.video.i_y_offset / 2) * src->p[i].i_pitch;
    }
    err = vdp_video_surface_put_bits_y_cb_cr(sys->vdp, surface, sys->format,
                                             planes, pitches);
    if (err != VDP_STATUS_OK)
    {
        msg_Err(filter, "video %s %s failure: %s", "surface", "import",
                vdp_get_error_string(sys->vdp, err));
        goto error;
    }
    /* Wrap surface into a picture */
    video_format_t fmt = src->format;
    fmt.i_chroma = (sys->chroma == VDP_CHROMA_TYPE_420)
        ? VLC_CODEC_VDPAU_VIDEO_420 : VLC_CODEC_VDPAU_VIDEO_422;
    picture_t *dst = picture_NewFromFormat(&fmt);
    if (unlikely(dst == NULL))
        goto error;
    picture_CopyProperties(dst, src);
    picture_Release(src);
    err = vlc_vdp_video_attach(sys->vdp, surface, dst);
    if (unlikely(err != VDP_STATUS_OK))
    {
        picture_Release(dst);
        dst = NULL;
    }
    return dst;
error:
    vdp_video_surface_destroy(sys->vdp, surface);
drop:
    picture_Release(src);
    return NULL;
}
