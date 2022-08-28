int av_frame_ref(AVFrame *dst, const AVFrame *src)
{
    int i, ret = 0;
    dst->format         = src->format;
    dst->width          = src->width;
    dst->height         = src->height;
    dst->channels       = src->channels;
    dst->channel_layout = src->channel_layout;
    dst->nb_samples     = src->nb_samples;
    ret = frame_copy_props(dst, src, 0);
    if (ret < 0)
        return ret;
    
    if (!src->buf[0]) {
        ret = av_frame_get_buffer(dst, 32);
        if (ret < 0)
            return ret;
        ret = av_frame_copy(dst, src);
        if (ret < 0)
            av_frame_unref(dst);
        return ret;
    }
    
    for (i = 0; i < FF_ARRAY_ELEMS(src->buf); i++) {
        if (!src->buf[i])
            continue;
        dst->buf[i] = av_buffer_ref(src->buf[i]);
        if (!dst->buf[i]) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
    }
    if (src->extended_buf) {
        dst->extended_buf = av_mallocz_array(sizeof(*dst->extended_buf),
                                       src->nb_extended_buf);
        if (!dst->extended_buf) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        dst->nb_extended_buf = src->nb_extended_buf;
        for (i = 0; i < src->nb_extended_buf; i++) {
            dst->extended_buf[i] = av_buffer_ref(src->extended_buf[i]);
            if (!dst->extended_buf[i]) {
                ret = AVERROR(ENOMEM);
                goto fail;
            }
        }
    }
    
    if (src->extended_data != src->data) {
        int ch = src->channels;
        if (!ch) {
            ret = AVERROR(EINVAL);
            goto fail;
        }
        CHECK_CHANNELS_CONSISTENCY(src);
        dst->extended_data = av_malloc_array(sizeof(*dst->extended_data), ch);
        if (!dst->extended_data) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        memcpy(dst->extended_data, src->extended_data, sizeof(*src->extended_data) * ch);
    } else
        dst->extended_data = dst->data;
    memcpy(dst->data,     src->data,     sizeof(src->data));
    memcpy(dst->linesize, src->linesize, sizeof(src->linesize));
    return 0;
fail:
    av_frame_unref(dst);
    return ret;
}
