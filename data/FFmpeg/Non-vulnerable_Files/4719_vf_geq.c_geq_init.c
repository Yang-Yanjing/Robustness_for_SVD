static av_cold int geq_init(AVFilterContext *ctx)
{
    GEQContext *geq = ctx->priv;
    int plane, ret = 0;
    if (!geq->expr_str[Y] && !geq->expr_str[G] && !geq->expr_str[B] && !geq->expr_str[R]) {
        av_log(ctx, AV_LOG_ERROR, "A luminance or RGB expression is mandatory\n");
        ret = AVERROR(EINVAL);
        goto end;
    }
    geq->is_rgb = !geq->expr_str[Y];
    if ((geq->expr_str[Y] || geq->expr_str[U] || geq->expr_str[V]) && (geq->expr_str[G] || geq->expr_str[B] || geq->expr_str[R])) {
        av_log(ctx, AV_LOG_ERROR, "Either YCbCr or RGB but not both must be specified\n");
        ret = AVERROR(EINVAL);
        goto end;
    }
    if (!geq->expr_str[U] && !geq->expr_str[V]) {
        
        geq->expr_str[U] = av_strdup(geq->expr_str[Y]);
        geq->expr_str[V] = av_strdup(geq->expr_str[Y]);
    } else {
        
        if (!geq->expr_str[U]) geq->expr_str[U] = av_strdup(geq->expr_str[V]);
        if (!geq->expr_str[V]) geq->expr_str[V] = av_strdup(geq->expr_str[U]);
    }
    if (!geq->expr_str[A])
        geq->expr_str[A] = av_strdup("255");
    if (!geq->expr_str[G])
        geq->expr_str[G] = av_strdup("g(X,Y)");
    if (!geq->expr_str[B])
        geq->expr_str[B] = av_strdup("b(X,Y)");
    if (!geq->expr_str[R])
        geq->expr_str[R] = av_strdup("r(X,Y)");
    if (geq->is_rgb ?
            (!geq->expr_str[G] || !geq->expr_str[B] || !geq->expr_str[R])
                    :
            (!geq->expr_str[U] || !geq->expr_str[V] || !geq->expr_str[A])) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    for (plane = 0; plane < 4; plane++) {
        static double (*p[])(void *, double, double) = { lum, cb, cr, alpha };
        static const char *const func2_yuv_names[]    = { "lum", "cb", "cr", "alpha", "p", NULL };
        static const char *const func2_rgb_names[]    = { "g", "b", "r", "alpha", "p", NULL };
        const char *const *func2_names       = geq->is_rgb ? func2_rgb_names : func2_yuv_names;
        double (*func2[])(void *, double, double) = { lum, cb, cr, alpha, p[plane], NULL };
        ret = av_expr_parse(&geq->e[plane], geq->expr_str[plane < 3 && geq->is_rgb ? plane+4 : plane], var_names,
                            NULL, NULL, func2_names, func2, 0, ctx);
        if (ret < 0)
            break;
    }
end:
    return ret;
}
