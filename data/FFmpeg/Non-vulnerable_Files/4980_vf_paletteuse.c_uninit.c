static av_cold void uninit(AVFilterContext *ctx)
{
    int i;
    PaletteUseContext *s = ctx->priv;
    ff_dualinput_uninit(&s->dinput);
    for (i = 0; i < CACHE_SIZE; i++)
        av_freep(&s->cache[i].entries);
    av_frame_free(&s->last_in);
    av_frame_free(&s->last_out);
}
