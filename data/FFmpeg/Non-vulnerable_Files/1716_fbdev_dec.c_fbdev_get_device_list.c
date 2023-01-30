static int fbdev_get_device_list(AVFormatContext *s, AVDeviceInfoList *device_list)
{
    return ff_fbdev_get_device_list(device_list);
}
