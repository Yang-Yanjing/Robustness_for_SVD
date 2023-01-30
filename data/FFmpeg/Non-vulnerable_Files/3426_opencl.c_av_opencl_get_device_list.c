int av_opencl_get_device_list(AVOpenCLDeviceList **device_list)
{
    int ret = 0;
    *device_list = av_mallocz(sizeof(AVOpenCLDeviceList));
    if (!(*device_list)) {
        av_log(&opencl_ctx, AV_LOG_ERROR, "Could not allocate opencl device list\n");
        return AVERROR(ENOMEM);
    }
    ret = get_device_list(*device_list);
    if (ret < 0) {
        av_log(&opencl_ctx, AV_LOG_ERROR, "Could not get device list from environment\n");
        free_device_list(*device_list);
        av_freep(device_list);
        return ret;
    }
    return ret;
}
