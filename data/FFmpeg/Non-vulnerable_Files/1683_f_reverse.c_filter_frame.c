static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    ReverseContext *s    = ctx->priv;
    void *ptr;
    if (s->nb_frames + 1 > s->pts_size / sizeof(*(s->pts))) {
        ptr = av_fast_realloc(s->pts, &s->pts_size, s->pts_size * 2);
        if (!ptr)
            return AVERROR(ENOMEM);
        s->pts = ptr;
    }
    if (s->nb_frames + 1 > s->frames_size / sizeof(*(s->frames))) {
        ptr = av_fast_realloc(s->frames, &s->frames_size, s->frames_size * 2);
        if (!ptr)
            return AVERROR(ENOMEM);
        s->frames = ptr;
    }
    s->frames[s->nb_frames] = in;
    s->pts[s->nb_frames]    = in->pts;
    s->nb_frames++;
    return 0;
}
