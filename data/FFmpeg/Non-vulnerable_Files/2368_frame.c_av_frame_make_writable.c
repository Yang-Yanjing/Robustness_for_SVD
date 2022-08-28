int av_frame_make_writable(AVFrame *frame)
{
    AVFrame tmp;
    int ret;
    if (!frame->buf[0])
        return AVERROR(EINVAL);
    if (av_frame_is_writable(frame))
        return 0;
    memset(&tmp, 0, sizeof(tmp));
    tmp.format         = frame->format;
    tmp.width          = frame->width;
    tmp.height         = frame->height;
    tmp.channels       = frame->channels;
    tmp.channel_layout = frame->channel_layout;
    tmp.nb_samples     = frame->nb_samples;
    ret = av_frame_get_buffer(&tmp, 32);
    if (ret < 0)
        return ret;
    ret = av_frame_copy(&tmp, frame);
    if (ret < 0) {
        av_frame_unref(&tmp);
        return ret;
    }
    ret = av_frame_copy_props(&tmp, frame);
    if (ret < 0) {
        av_frame_unref(&tmp);
        return ret;
    }
    av_frame_unref(frame);
    *frame = tmp;
    if (tmp.data == tmp.extended_data)
        frame->extended_data = frame->data;
    return 0;
}
