AVFILTER_DEFINE_CLASS(random);
static av_cold int init(AVFilterContext *ctx)
{
    RandomContext *s = ctx->priv;
    uint32_t seed;
    if (s->random_seed < 0)
        s->random_seed = av_get_random_seed();
    seed = s->random_seed;
    av_lfg_init(&s->lfg, seed);
    return 0;
}
