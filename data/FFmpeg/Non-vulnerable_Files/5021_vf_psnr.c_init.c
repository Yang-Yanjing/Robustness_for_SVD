static av_cold int init(AVFilterContext *ctx)
{
    PSNRContext *s = ctx->priv;
    s->min_mse = +INFINITY;
    s->max_mse = -INFINITY;
    if (s->stats_file_str) {
        s->stats_file = fopen(s->stats_file_str, "w");
        if (!s->stats_file) {
            int err = AVERROR(errno);
            char buf[128];
            av_strerror(err, buf, sizeof(buf));
            av_log(ctx, AV_LOG_ERROR, "Could not open stats file %s: %s\n",
                   s->stats_file_str, buf);
            return err;
        }
    }
    s->dinput.process = do_psnr;
    return 0;
}
