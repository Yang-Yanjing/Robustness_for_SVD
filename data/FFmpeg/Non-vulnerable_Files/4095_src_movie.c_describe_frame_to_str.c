static char *describe_frame_to_str(char *dst, size_t dst_size,
                                   AVFrame *frame, enum AVMediaType frame_type,
                                   AVFilterLink *link)
{
    switch (frame_type) {
    case AVMEDIA_TYPE_VIDEO:
        snprintf(dst, dst_size,
                 "video pts:%s time:%s size:%dx%d aspect:%d/%d",
                 av_ts2str(frame->pts), av_ts2timestr(frame->pts, &link->time_base),
                 frame->width, frame->height,
                 frame->sample_aspect_ratio.num,
                 frame->sample_aspect_ratio.den);
                 break;
    case AVMEDIA_TYPE_AUDIO:
        snprintf(dst, dst_size,
                 "audio pts:%s time:%s samples:%d",
                 av_ts2str(frame->pts), av_ts2timestr(frame->pts, &link->time_base),
                 frame->nb_samples);
                 break;
    default:
        snprintf(dst, dst_size, "%s BUG", av_get_media_type_string(frame_type));
        break;
    }
    return dst;
}
