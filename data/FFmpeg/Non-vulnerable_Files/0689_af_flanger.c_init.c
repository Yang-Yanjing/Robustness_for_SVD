AVFILTER_DEFINE_CLASS(flanger);
static int init(AVFilterContext *ctx)
{
    FlangerContext *s = ctx->priv;
    s->feedback_gain /= 100;
    s->delay_gain    /= 100;
    s->channel_phase /= 100;
    s->delay_min     /= 1000;
    s->delay_depth   /= 1000;
    s->in_gain        = 1 / (1 + s->delay_gain);
    s->delay_gain    /= 1 + s->delay_gain;
    s->delay_gain    *= 1 - fabs(s->feedback_gain);
    return 0;
}
