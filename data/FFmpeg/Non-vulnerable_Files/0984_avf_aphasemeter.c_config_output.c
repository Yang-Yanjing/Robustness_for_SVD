static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AudioPhaseMeterContext *s = ctx->priv;
    outlink->w = s->w;
    outlink->h = s->h;
    outlink->sample_aspect_ratio = (AVRational){1,1};
    outlink->frame_rate = s->frame_rate;
    if (!strcmp(s->mpc_str, "none"))
        s->draw_median_phase = 0;
    else if (av_parse_color(s->mpc, s->mpc_str, -1, ctx) >= 0)
        s->draw_median_phase = 1;
    else
        return AVERROR(EINVAL);
    return 0;
}
