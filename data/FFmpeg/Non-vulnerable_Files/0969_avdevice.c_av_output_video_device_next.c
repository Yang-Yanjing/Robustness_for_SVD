AVOutputFormat *av_output_video_device_next(AVOutputFormat *d)
{
    return device_next(d, 1, AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT,
                       AV_CLASS_CATEGORY_DEVICE_OUTPUT);
}
