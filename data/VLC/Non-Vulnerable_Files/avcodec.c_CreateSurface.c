};
static vlc_vdp_video_field_t *CreateSurface(vlc_va_t *va)
{
    vlc_va_sys_t *sys = va->sys;
    VdpVideoSurface surface;
    VdpStatus err;
    err = vdp_video_surface_create(sys->vdp, sys->device, VDP_CHROMA_TYPE_420,
                                   sys->width, sys->height, &surface);
    if (err != VDP_STATUS_OK)
    {
        msg_Err(va, "%s creation failure: %s", "video surface",
                vdp_get_error_string(sys->vdp, err));
        return NULL;
    }
    vlc_vdp_video_field_t *field = vlc_vdp_video_create(sys->vdp, surface);
    if (unlikely(field == NULL))
        vdp_video_surface_destroy(sys->vdp, surface);
    return field;
}
