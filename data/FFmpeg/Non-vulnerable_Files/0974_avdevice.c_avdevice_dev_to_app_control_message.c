int avdevice_dev_to_app_control_message(struct AVFormatContext *s, enum AVDevToAppMessageType type,
                                        void *data, size_t data_size)
{
    if (!av_format_get_control_message_cb(s))
        return AVERROR(ENOSYS);
    return av_format_get_control_message_cb(s)(s, type, data, data_size);
}
