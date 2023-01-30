int avdevice_list_input_sources(AVInputFormat *device, const char *device_name,
                                AVDictionary *device_options, AVDeviceInfoList **device_list)
{
    AVFormatContext *s = NULL;
    int ret;
    if ((ret = ff_alloc_input_device_context(&s, device, device_name)) < 0)
        return ret;
    return list_devices_for_context(s, device_options, device_list);
}
