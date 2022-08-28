void avdevice_free_list_devices(AVDeviceInfoList **device_list)
{
    AVDeviceInfoList *list;
    AVDeviceInfo *dev;
    int i;
    av_assert0(device_list);
    list = *device_list;
    if (!list)
        return;
    for (i = 0; i < list->nb_devices; i++) {
        dev = list->devices[i];
        if (dev) {
            av_freep(&dev->device_name);
            av_freep(&dev->device_description);
            av_free(dev);
        }
    }
    av_freep(&list->devices);
    av_freep(device_list);
}
