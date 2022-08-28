static int print_device_sinks(AVOutputFormat *fmt, AVDictionary *opts)
{
    int ret, i;
    AVDeviceInfoList *device_list = NULL;
    if (!fmt || !fmt->priv_class  || !AV_IS_OUTPUT_DEVICE(fmt->priv_class->category))
        return AVERROR(EINVAL);
    printf("Auto-detected sinks for %s:\n", fmt->name);
    if (!fmt->get_device_list) {
        ret = AVERROR(ENOSYS);
        printf("Cannot list sinks. Not implemented.\n");
        goto fail;
    }
    if ((ret = avdevice_list_output_sinks(fmt, NULL, opts, &device_list)) < 0) {
        printf("Cannot list sinks.\n");
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
