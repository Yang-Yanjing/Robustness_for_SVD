}
static inline VdpVideoSurface picture_GetVideoSurface(const picture_t *pic)
{
    vlc_vdp_video_field_t *field = pic->context;
    return field->frame->surface;
}
