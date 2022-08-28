void av_opencl_free_device_list(AVOpenCLDeviceList **device_list)
{
    free_device_list(*device_list);
    av_freep(device_list);
}
