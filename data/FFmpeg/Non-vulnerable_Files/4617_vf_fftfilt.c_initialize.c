static av_cold int initialize(AVFilterContext *ctx)
{
    FFTFILTContext *fftfilt = ctx->priv;
    int ret = 0, plane;
    if (!fftfilt->dc[U] && !fftfilt->dc[V]) {
        fftfilt->dc[U] = fftfilt->dc[Y];
        fftfilt->dc[V] = fftfilt->dc[Y];
    } else {
        if (!fftfilt->dc[U]) fftfilt->dc[U] = fftfilt->dc[V];
        if (!fftfilt->dc[V]) fftfilt->dc[V] = fftfilt->dc[U];
    }
    if (!fftfilt->weight_str[U] && !fftfilt->weight_str[V]) {
        fftfilt->weight_str[U] = av_strdup(fftfilt->weight_str[Y]);
        fftfilt->weight_str[V] = av_strdup(fftfilt->weight_str[Y]);
    } else {
        if (!fftfilt->weight_str[U]) fftfilt->weight_str[U] = av_strdup(fftfilt->weight_str[V]);
        if (!fftfilt->weight_str[V]) fftfilt->weight_str[V] = av_strdup(fftfilt->weight_str[U]);
    }
    for (plane = 0; plane < 3; plane++) {
        static double (*p[])(void *, double, double) = { weight_Y, weight_U, weight_V };
        const char *const func2_names[] = {"weight_Y", "weight_U", "weight_V", NULL };
        double (*func2[])(void *, double, double) = { weight_Y, weight_U, weight_V, p[plane], NULL };
        ret = av_expr_parse(&fftfilt->weight_expr[plane], fftfilt->weight_str[plane], var_names,
                            NULL, NULL, func2_names, func2, 0, ctx);
        if (ret < 0)
            break;
    }
    return ret;
}
