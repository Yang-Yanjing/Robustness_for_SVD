int ff_filter_frame(AVFilterLink *link, AVFrame *frame)
{
    FF_TPRINTF_START(NULL, filter_frame); ff_tlog_link(NULL, link, 1); ff_tlog(NULL, " "); ff_tlog_ref(NULL, frame, 1);
    
    if (link->type == AVMEDIA_TYPE_VIDEO) {
        if (strcmp(link->dst->filter->name, "buffersink") &&
            strcmp(link->dst->filter->name, "format") &&
            strcmp(link->dst->filter->name, "idet") &&
            strcmp(link->dst->filter->name, "null") &&
            strcmp(link->dst->filter->name, "scale")) {
            av_assert1(frame->format                 == link->format);
            av_assert1(frame->width               == link->w);
            av_assert1(frame->height               == link->h);
        }
    } else {
        av_assert1(frame->format                == link->format);
        av_assert1(av_frame_get_channels(frame) == link->channels);
        av_assert1(frame->channel_layout        == link->channel_layout);
        av_assert1(frame->sample_rate           == link->sample_rate);
    }
    
    if (link->type == AVMEDIA_TYPE_AUDIO &&
        link->min_samples &&
        (link->partial_buf ||
         frame->nb_samples < link->min_samples ||
         frame->nb_samples > link->max_samples)) {
        return ff_filter_frame_needs_framing(link, frame);
    } else {
        return ff_filter_frame_framed(link, frame);
    }
}
