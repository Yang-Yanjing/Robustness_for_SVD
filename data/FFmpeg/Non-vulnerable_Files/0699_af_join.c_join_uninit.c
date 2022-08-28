static av_cold void join_uninit(AVFilterContext *ctx)
{
    JoinContext *s = ctx->priv;
    int i;
    for (i = 0; i < ctx->nb_inputs; i++) {
        av_freep(&ctx->input_pads[i].name);
        av_frame_free(&s->input_frames[i]);
    }
    av_freep(&s->channels);
    av_freep(&s->buffers);
    av_freep(&s->input_frames);
}
