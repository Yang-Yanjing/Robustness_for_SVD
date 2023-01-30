}
static void DestroySurface(vlc_vdp_video_field_t *field)
{
    assert(field != NULL);
    field->destroy(field);
}
