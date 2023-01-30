AVBufferRef *av_frame_get_plane_buffer(AVFrame *frame, int plane)
{
    uint8_t *data;
    int planes, i;
    if (frame->nb_samples) {
        int channels = frame->channels;
        if (!channels)
            return NULL;
        CHECK_CHANNELS_CONSISTENCY(frame);
        planes = av_sample_fmt_is_planar(frame->format) ? channels : 1;
    } else
        planes = 4;
    if (plane < 0 || plane >= planes || !frame->extended_data[plane])
        return NULL;
    data = frame->extended_data[plane];
    for (i = 0; i < FF_ARRAY_ELEMS(frame->buf) && frame->buf[i]; i++) {
        AVBufferRef *buf = frame->buf[i];
        if (data >= buf->data && data < buf->data + buf->size)
            return buf;
    }
    for (i = 0; i < frame->nb_extended_buf; i++) {
        AVBufferRef *buf = frame->extended_buf[i];
        if (data >= buf->data && data < buf->data + buf->size)
            return buf;
    }
    return NULL;
}
