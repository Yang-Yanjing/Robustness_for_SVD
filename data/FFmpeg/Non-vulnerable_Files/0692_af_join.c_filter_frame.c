AVFILTER_DEFINE_CLASS(join);
static int filter_frame(AVFilterLink *link, AVFrame *frame)
{
    AVFilterContext *ctx = link->dst;
    JoinContext       *s = ctx->priv;
    int i;
    for (i = 0; i < ctx->nb_inputs; i++)
        if (link == ctx->inputs[i])
            break;
    av_assert0(i < ctx->nb_inputs);
    av_assert0(!s->input_frames[i]);
    s->input_frames[i] = frame;
    return 0;
}
