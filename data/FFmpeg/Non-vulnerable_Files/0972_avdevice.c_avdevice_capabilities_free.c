void avdevice_capabilities_free(AVDeviceCapabilitiesQuery **caps, AVFormatContext *s)
{
    if (!s || !caps || !(*caps))
        return;
    av_assert0(s->iformat || s->oformat);
    if (s->iformat) {
        if (s->iformat->free_device_capabilities)
            s->iformat->free_device_capabilities(s, *caps);
    } else {
        if (s->oformat->free_device_capabilities)
            s->oformat->free_device_capabilities(s, *caps);
    }
    av_freep(caps);
}
