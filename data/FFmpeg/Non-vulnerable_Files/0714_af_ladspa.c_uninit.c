static av_cold void uninit(AVFilterContext *ctx)
{
    LADSPAContext *s = ctx->priv;
    int i;
    for (i = 0; i < s->nb_handles; i++) {
        if (s->desc->deactivate)
            s->desc->deactivate(s->handles[i]);
        if (s->desc->cleanup)
            s->desc->cleanup(s->handles[i]);
    }
    if (s->dl_handle)
        dlclose(s->dl_handle);
    av_freep(&s->ipmap);
    av_freep(&s->opmap);
    av_freep(&s->icmap);
    av_freep(&s->ocmap);
    av_freep(&s->ictlv);
    av_freep(&s->octlv);
    av_freep(&s->handles);
    av_freep(&s->ctl_needs_value);
    if (ctx->nb_inputs)
        av_freep(&ctx->input_pads[0].name);
}
