int avfilter_copy_frame_props(AVFilterBufferRef *dst, const AVFrame *src)
{
    dst->pts    = src->pts;
    dst->pos    = av_frame_get_pkt_pos(src);
    dst->format = src->format;
    av_dict_free(&dst->metadata);
    av_dict_copy(&dst->metadata, av_frame_get_metadata(src), 0);
    switch (dst->type) {
    case AVMEDIA_TYPE_VIDEO:
        dst->video->w                   = src->width;
        dst->video->h                   = src->height;
        dst->video->sample_aspect_ratio = src->sample_aspect_ratio;
        dst->video->interlaced          = src->interlaced_frame;
        dst->video->top_field_first     = src->top_field_first;
        dst->video->key_frame           = src->key_frame;
        dst->video->pict_type           = src->pict_type;
        break;
    case AVMEDIA_TYPE_AUDIO:
        dst->audio->sample_rate         = src->sample_rate;
        dst->audio->channel_layout      = src->channel_layout;
        break;
    default:
        return AVERROR(EINVAL);
    }
    return 0;
}
