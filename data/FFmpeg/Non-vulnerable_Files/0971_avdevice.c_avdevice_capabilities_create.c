int avdevice_capabilities_create(AVDeviceCapabilitiesQuery **caps, AVFormatContext *s,
                                 AVDictionary **device_options)
{
    int ret;
    av_assert0(s && caps);
    av_assert0(s->iformat || s->oformat);
    if ((s->oformat && !s->oformat->create_device_capabilities) ||
        (s->iformat && !s->iformat->create_device_capabilities))
        return AVERROR(ENOSYS);
    *caps = av_mallocz(sizeof(**caps));
    if (!(*caps))
        return AVERROR(ENOMEM);
    (*caps)->device_context = s;
    if (((ret = av_opt_set_dict(s->priv_data, device_options)) < 0))
        goto fail;
    if (s->iformat) {
        if ((ret = s->iformat->create_device_capabilities(s, *caps)) < 0)
            goto fail;
    } else {
        if ((ret = s->oformat->create_device_capabilities(s, *caps)) < 0)
            goto fail;
    }
    av_opt_set_defaults(*caps);
    return 0;
  fail:
    av_freep(caps);
    return ret;
}
