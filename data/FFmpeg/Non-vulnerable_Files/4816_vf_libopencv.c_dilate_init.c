static av_cold int dilate_init(AVFilterContext *ctx, const char *args)
{
    OCVContext *s = ctx->priv;
    DilateContext *dilate = s->priv;
    char default_kernel_str[] = "3x3+0x0/rect";
    char *kernel_str = NULL;
    const char *buf = args;
    int ret;
    if (args) {
        kernel_str = av_get_token(&buf, "|");
        if (!kernel_str)
            return AVERROR(ENOMEM);
    }
    ret = parse_iplconvkernel(&dilate->kernel,
                              (!kernel_str || !*kernel_str) ? default_kernel_str
                                                            : kernel_str,
                              ctx);
    av_free(kernel_str);
    if (ret < 0)
        return ret;
    if (!buf || sscanf(buf, "|%d", &dilate->nb_iterations) != 1)
        dilate->nb_iterations = 1;
    av_log(ctx, AV_LOG_VERBOSE, "iterations_nb:%d\n", dilate->nb_iterations);
    if (dilate->nb_iterations <= 0) {
        av_log(ctx, AV_LOG_ERROR, "Invalid non-positive value '%d' for nb_iterations\n",
               dilate->nb_iterations);
        return AVERROR(EINVAL);
    }
    return 0;
}
