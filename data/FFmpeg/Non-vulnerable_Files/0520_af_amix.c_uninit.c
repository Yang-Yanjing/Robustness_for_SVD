static av_cold void uninit(AVFilterContext *ctx)
{
    int i;
    MixContext *s = ctx->priv;
    if (s->fifos) {
        for (i = 0; i < s->nb_inputs; i++)
            av_audio_fifo_free(s->fifos[i]);
        av_freep(&s->fifos);
    }
    frame_list_clear(s->frame_list);
    av_freep(&s->frame_list);
    av_freep(&s->input_state);
    av_freep(&s->input_scale);
    av_freep(&s->fdsp);
    for (i = 0; i < ctx->nb_inputs; i++)
        av_freep(&ctx->input_pads[i].name);
}
