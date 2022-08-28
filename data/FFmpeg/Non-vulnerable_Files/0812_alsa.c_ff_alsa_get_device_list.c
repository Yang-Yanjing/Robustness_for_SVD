int ff_alsa_get_device_list(AVDeviceInfoList *device_list, snd_pcm_stream_t stream_type)
{
    int ret = 0;
    void **hints, **n;
    char *name = NULL, *descr = NULL, *io = NULL, *tmp;
    AVDeviceInfo *new_device = NULL;
    const char *filter = stream_type == SND_PCM_STREAM_PLAYBACK ? "Output" : "Input";
    if (snd_device_name_hint(-1, "pcm", &hints) < 0)
        return AVERROR_EXTERNAL;
    n = hints;
    while (*n && !ret) {
        name = snd_device_name_get_hint(*n, "NAME");
        descr = snd_device_name_get_hint(*n, "DESC");
        io = snd_device_name_get_hint(*n, "IOID");
        if (!io || !strcmp(io, filter)) {
            new_device = av_mallocz(sizeof(AVDeviceInfo));
            if (!new_device) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
            new_device->device_name = av_strdup(name);
            if ((tmp = strrchr(descr, '\n')) && tmp[1])
                new_device->device_description = av_strdup(&tmp[1]);
            else
                new_device->device_description = av_strdup(descr);
            if (!new_device->device_description || !new_device->device_name) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
            if ((ret = av_dynarray_add_nofree(&device_list->devices,
                                              &device_list->nb_devices, new_device)) < 0) {
                goto fail;
            }
            if (!strcmp(new_device->device_name, "default"))
                device_list->default_device = device_list->nb_devices - 1;
            new_device = NULL;
        }
      fail:
        free(io);
        free(name);
        free(descr);
        n++;
    }
    if (new_device) {
        av_free(new_device->device_description);
        av_free(new_device->device_name);
        av_free(new_device);
    }
    snd_device_name_free_hint(hints);
    return ret;
}
