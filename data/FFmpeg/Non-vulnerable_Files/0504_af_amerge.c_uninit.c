AVFILTER_DEFINE_CLASS(amerge);
static av_cold void uninit(AVFilterContext *ctx)
{
    AMergeContext *s = ctx->priv;
    int i;
    for (i = 0; i < s->nb_inputs; i++) {
        if (s->in)
            ff_bufqueue_discard_all(&s->in[i].queue);
        if (ctx->input_pads)
            av_freep(&ctx->input_pads[i].name);
    }
    av_freep(&s->in);
}
