/** Export a VDPAU video surface picture to a normal VLC picture */
static picture_t *VideoExport(filter_t *filter, picture_t *src, picture_t *dst)
{
    filter_sys_t *sys = filter->p_sys;
    vlc_vdp_video_field_t *field = src->context;
    vlc_vdp_video_frame_t *psys = field->frame;
    VdpStatus err;
    VdpVideoSurface surface = psys->surface;
    void *planes[3];
    uint32_t pitches[3];
    picture_CopyProperties(dst, src);
    for (int i = 0; i < dst->i_planes; i++)
    {
        planes[i] = dst->p[i].p_pixels;
        pitches[i] = dst->p[i].i_pitch;
    }
    if (dst->format.i_chroma == VLC_CODEC_I420
     || dst->format.i_chroma == VLC_CODEC_I422)
    {
        planes[1] = dst->p[2].p_pixels;
        planes[2] = dst->p[1].p_pixels;
        pitches[1] = dst->p[2].i_pitch;
        pitches[2] = dst->p[1].i_pitch;
    }
    err = vdp_video_surface_get_bits_y_cb_cr(psys->vdp, surface, sys->format,
                                             planes, pitches);
    if (err != VDP_STATUS_OK)
    {
        msg_Err(filter, "video %s %s failure: %s", "surface", "export",
                vdp_get_error_string(psys->vdp, err));
        picture_Release(dst);
        dst = NULL;
    }
    picture_Release(src);
    return dst;
}
