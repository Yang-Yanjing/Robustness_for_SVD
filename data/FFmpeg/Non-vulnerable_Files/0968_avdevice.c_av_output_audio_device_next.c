AVOutputFormat *av_output_audio_device_next(AVOutputFormat *d)
{
    return device_next(d, 1, AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT,
                       AV_CLASS_CATEGORY_DEVICE_OUTPUT);
}
