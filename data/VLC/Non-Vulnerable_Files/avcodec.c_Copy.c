}
static int Copy(vlc_va_t *va, picture_t *pic, void *opaque, uint8_t *data)
{
    vlc_vdp_video_field_t *field = opaque;
    assert(field != NULL);
    field = vlc_vdp_video_copy(field);
    if (unlikely(field == NULL))
        return VLC_ENOMEM;
    assert(pic->context == NULL);
    pic->context = field;
    (void) va; (void) data;
    return VLC_SUCCESS;
}
