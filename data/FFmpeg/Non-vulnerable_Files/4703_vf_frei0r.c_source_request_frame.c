static int source_request_frame(AVFilterLink *outlink)
{
    Frei0rContext *s = outlink->src->priv;
    AVFrame *frame = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!frame)
        return AVERROR(ENOMEM);
    frame->sample_aspect_ratio = (AVRational) {1, 1};
    frame->pts = s->pts++;
    s->update(s->instance, av_rescale_q(frame->pts, s->time_base, (AVRational){1,1000}),
                   NULL, (uint32_t *)frame->data[0]);
    return ff_filter_frame(outlink, frame);
}
