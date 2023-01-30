static int print_device_sources(AVInputFormat *fmt, AVDictionary *opts)
{
    int ret, i;
    AVDeviceInfoList *device_list = NULL;
    if (!fmt || !fmt->priv_class  || !AV_IS_INPUT_DEVICE(fmt->priv_class->category))
        return AVERROR(EINVAL);
    printf("Auto-detected sources for %s:\n", fmt->name);
    if (!fmt->get_device_list) {
        ret = AVERROR(ENOSYS);
        printf("Cannot list sources. Not implemented.\n");
        goto fail;
    }
    if ((ret = avdevice_list_input_sources(fmt, NULL, opts, &device_list)) < 0) {
        printf("Cannot list sources.\n");
        goto fail;
    }
    for (i = 0; i < device_list->nb_devices; i++) {
        printf("%s %s [%s]\n", device_list->default_device == i ? "*" : " ",
               device_list->devices[i]->device_name, device_list->devices[i]->device_description);
    }
  fail:
    avdevice_free_list_devices(&device_list);
    return ret;
}
