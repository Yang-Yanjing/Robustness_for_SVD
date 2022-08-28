int avfilter_copy_buf_props(AVFrame *dst, const AVFilterBufferRef *src)
{
    int planes, nb_channels;
    if (!dst)
        return AVERROR(EINVAL);
    
    av_assert0(src);
    memcpy(dst->data, src->data, sizeof(dst->data));
    memcpy(dst->linesize, src->linesize, sizeof(dst->linesize));
    dst->pts     = src->pts;
    dst->format  = src->format;
    av_frame_set_pkt_pos(dst, src->pos);
    switch (src->type) {
    case AVMEDIA_TYPE_VIDEO:
        av_assert0(src->video);
        dst->width               = src->video->w;
        dst->height              = src->video->h;
        dst->sample_aspect_ratio = src->video->sample_aspect_ratio;
        dst->interlaced_frame    = src->video->interlaced;
        dst->top_field_first     = src->video->top_field_first;
        dst->key_frame           = src->video->key_frame;
        dst->pict_type           = src->video->pict_type;
        break;
    case AVMEDIA_TYPE_AUDIO:
        av_assert0(src->audio);
        nb_channels = av_get_channel_layout_nb_channels(src->audio->channel_layout);
        planes      = av_sample_fmt_is_planar(src->format) ? nb_channels : 1;
        if (planes > FF_ARRAY_ELEMS(dst->data)) {
            dst->extended_data = av_mallocz_array(planes, sizeof(*dst->extended_data));
            if (!dst->extended_data)
                return AVERROR(ENOMEM);
            memcpy(dst->extended_data, src->extended_data,
                   planes * sizeof(*dst->extended_data));
        } else
            dst->extended_data = dst->data;
        dst->nb_samples          = src->audio->nb_samples;
        av_frame_set_sample_rate   (dst, src->audio->sample_rate);
        av_frame_set_channel_layout(dst, src->audio->channel_layout);
        av_frame_set_channels      (dst, src->audio->channels);
        break;
    default:
        return AVERROR(EINVAL);
    }
    return 0;
}
