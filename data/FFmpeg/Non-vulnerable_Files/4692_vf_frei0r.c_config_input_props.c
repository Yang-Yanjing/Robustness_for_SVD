static int config_input_props(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    Frei0rContext *s = ctx->priv;
    if (s->destruct && s->instance)
        s->destruct(s->instance);
    if (!(s->instance = s->construct(inlink->w, inlink->h))) {
        av_log(ctx, AV_LOG_ERROR, "Impossible to load frei0r instance.\n");
        return AVERROR(EINVAL);
    }
    return set_params(ctx, s->params);
}
