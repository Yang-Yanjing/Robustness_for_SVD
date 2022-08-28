static av_cold int init(AVFilterContext *ctx)
{
    LifeContext *life = ctx->priv;
    int ret;
    if (!life->w && !life->filename)
        av_opt_set(life, "size", "320x240", 0);
    if ((ret = parse_rule(&life->born_rule, &life->stay_rule, life->rule_str, ctx)) < 0)
        return ret;
    if (!life->mold && memcmp(life->mold_color, "\x00\x00\x00", 3))
        av_log(ctx, AV_LOG_WARNING,
               "Mold color is set while mold isn't, ignoring the color.\n");
    if (!life->filename) {
        
        int i;
        if (!(life->buf[0] = av_calloc(life->h * life->w, sizeof(*life->buf[0]))) ||
            !(life->buf[1] = av_calloc(life->h * life->w, sizeof(*life->buf[1])))) {
            av_freep(&life->buf[0]);
            av_freep(&life->buf[1]);
            return AVERROR(ENOMEM);
        }
        if (life->random_seed == -1)
            life->random_seed = av_get_random_seed();
        av_lfg_init(&life->lfg, life->random_seed);
        for (i = 0; i < life->w * life->h; i++) {
            double r = (double)av_lfg_get(&life->lfg) / UINT32_MAX;
            if (r <= life->random_fill_ratio)
                life->buf[0][i] = ALIVE_CELL;
        }
        life->buf_idx = 0;
    } else {
        if ((ret = init_pattern_from_file(ctx)) < 0)
            return ret;
    }
    av_log(ctx, AV_LOG_VERBOSE,
           "s:%dx%d r:%d/%d rule:%s stay_rule:%d born_rule:%d stitch:%d seed:%u\n",
           life->w, life->h, life->frame_rate.num, life->frame_rate.den,
           life->rule_str, life->stay_rule, life->born_rule, life->stitch,
           life->random_seed);
    return 0;
}
