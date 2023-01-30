int avdevice_list_output_sinks(AVOutputFormat *device, const char *device_name,
                               AVDictionary *device_options, AVDeviceInfoList **device_list)
{
    AVFormatContext *s = NULL;
    int ret;
    if ((ret = avformat_alloc_output_context2(&s, device, device_name, NULL)) < 0)
        return ret;
    return list_devices_for_context(s, device_options, device_list);
}
