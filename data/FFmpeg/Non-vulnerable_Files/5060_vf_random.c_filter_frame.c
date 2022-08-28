static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    RandomContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;
    int idx;
    if (s->nb_frames_filled < s->nb_frames) {
        s->frames[s->nb_frames_filled] = in;
        s->pts[s->nb_frames_filled++] = in->pts;
        return 0;
    }
    idx = av_lfg_get(&s->lfg) % s->nb_frames;
    out = s->frames[idx];
    out->pts = s->pts[0];
    memmove(&s->pts[0], &s->pts[1], (s->nb_frames - 1) * sizeof(s->pts[0]));
    s->frames[idx] = in;
    s->pts[s->nb_frames - 1] = in->pts;
    return ff_filter_frame(outlink, out);
}
