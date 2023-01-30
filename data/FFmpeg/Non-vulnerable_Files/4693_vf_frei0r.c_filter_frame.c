static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    Frei0rContext *s = inlink->dst->priv;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    AVFrame *out;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    s->update(s->instance, in->pts * av_q2d(inlink->time_base) * 1000,
                   (const uint32_t *)in->data[0],
                   (uint32_t *)out->data[0]);
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
