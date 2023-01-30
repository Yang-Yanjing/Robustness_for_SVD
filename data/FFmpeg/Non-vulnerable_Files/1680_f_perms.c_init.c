static av_cold int init(AVFilterContext *ctx)
{
    PermsContext *perms = ctx->priv;
    if (perms->mode == MODE_RANDOM) {
        uint32_t seed;
        if (perms->random_seed == -1)
            perms->random_seed = av_get_random_seed();
        seed = perms->random_seed;
        av_log(ctx, AV_LOG_INFO, "random seed: 0x%08x\n", seed);
        av_lfg_init(&perms->lfg, seed);
    }
    return 0;
}
