int avdevice_list_devices(AVFormatContext *s, AVDeviceInfoList **device_list)
{
    int ret;
    av_assert0(s);
    av_assert0(device_list);
    av_assert0(s->oformat || s->iformat);
    if ((s->oformat && !s->oformat->get_device_list) ||
        (s->iformat && !s->iformat->get_device_list)) {
        *device_list = NULL;
        return AVERROR(ENOSYS);
    }
    *device_list = av_mallocz(sizeof(AVDeviceInfoList));
    if (!(*device_list))
        return AVERROR(ENOMEM);
    
    (*device_list)->default_device = -1;
    if (s->oformat)
        ret = s->oformat->get_device_list(s, *device_list);
    else
        ret = s->iformat->get_device_list(s, *device_list);
    if (ret < 0)
        avdevice_free_list_devices(device_list);
    return ret;
}
