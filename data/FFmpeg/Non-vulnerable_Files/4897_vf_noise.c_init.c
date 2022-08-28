static av_cold int init(AVFilterContext *ctx)
{
    NoiseContext *n = ctx->priv;
    int ret, i;
    for (i = 0; i < 4; i++) {
        if (n->all.seed >= 0)
            n->param[i].seed = n->all.seed;
        else
            n->param[i].seed = 123457;
        if (n->all.strength)
            n->param[i].strength = n->all.strength;
        if (n->all.flags)
            n->param[i].flags = n->all.flags;
    }
    for (i = 0; i < 4; i++) {
        if (n->param[i].strength && ((ret = init_noise(n, i)) < 0))
            return ret;
    }
    n->line_noise     = ff_line_noise_c;
    n->line_noise_avg = ff_line_noise_avg_c;
    if (ARCH_X86)
        ff_noise_init_x86(n);
    return 0;
}
