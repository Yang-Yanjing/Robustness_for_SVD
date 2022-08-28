static av_cold int init(AVFilterContext *ctx)
{
    CellAutoContext *cellauto = ctx->priv;
    int ret;
    if (!cellauto->w && !cellauto->filename && !cellauto->pattern)
        av_opt_set(cellauto, "size", "320x518", 0);
    if (cellauto->filename && cellauto->pattern) {
        av_log(ctx, AV_LOG_ERROR, "Only one of the filename or pattern options can be used\n");
        return AVERROR(EINVAL);
    }
    if (cellauto->filename) {
        if ((ret = init_pattern_from_file(ctx)) < 0)
            return ret;
    } else if (cellauto->pattern) {
        if ((ret = init_pattern_from_string(ctx)) < 0)
            return ret;
    } else {
        
        int i;
        cellauto->buf = av_mallocz_array(sizeof(uint8_t) * cellauto->w, cellauto->h);
        if (!cellauto->buf)
            return AVERROR(ENOMEM);
        if (cellauto->random_seed == -1)
            cellauto->random_seed = av_get_random_seed();
        av_lfg_init(&cellauto->lfg, cellauto->random_seed);
        for (i = 0; i < cellauto->w; i++) {
            double r = (double)av_lfg_get(&cellauto->lfg) / UINT32_MAX;
            if (r <= cellauto->random_fill_ratio)
                cellauto->buf[i] = 1;
        }
    }
    av_log(ctx, AV_LOG_VERBOSE,
           "s:%dx%d r:%d/%d rule:%d stitch:%d scroll:%d full:%d seed:%u\n",
           cellauto->w, cellauto->h, cellauto->frame_rate.num, cellauto->frame_rate.den,
           cellauto->rule, cellauto->stitch, cellauto->scroll, cellauto->start_full,
           cellauto->random_seed);
    return 0;
}
