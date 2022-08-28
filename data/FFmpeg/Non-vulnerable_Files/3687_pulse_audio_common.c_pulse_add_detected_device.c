static void pulse_add_detected_device(PulseAudioDeviceList *info,
                                      const char *name, const char *description)
{
    int ret;
    AVDeviceInfo *new_device = NULL;
    if (info->error_code)
        return;
    new_device = av_mallocz(sizeof(AVDeviceInfo));
    if (!new_device) {
        info->error_code = AVERROR(ENOMEM);
        return;
    }
    new_device->device_description = av_strdup(description);
    new_device->device_name = av_strdup(name);
    if (!new_device->device_description || !new_device->device_name) {
        info->error_code = AVERROR(ENOMEM);
        goto fail;
    }
    if ((ret = av_dynarray_add_nofree(&info->devices->devices,
                                      &info->devices->nb_devices, new_device)) < 0) {
        info->error_code = ret;
        goto fail;
    }
    return;
  fail:
    av_freep(&new_device->device_description);
    av_freep(&new_device->device_name);
    av_free(new_device);
}
