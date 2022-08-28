static av_cold void uninit(AVFilterContext *ctx)
{
    FliteContext *flite = ctx->priv;
    if (!--flite->voice_entry->usage_count)
        flite->voice_entry->unregister_fn(flite->voice);
    flite->voice = NULL;
    flite->voice_entry = NULL;
    delete_wave(flite->wave);
    flite->wave = NULL;
}
