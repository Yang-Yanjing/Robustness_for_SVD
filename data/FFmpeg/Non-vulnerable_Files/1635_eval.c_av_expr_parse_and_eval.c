int av_expr_parse_and_eval(double *d, const char *s,
                           const char * const *const_names, const double *const_values,
                           const char * const *func1_names, double (* const *funcs1)(void *, double),
                           const char * const *func2_names, double (* const *funcs2)(void *, double, double),
                           void *opaque, int log_offset, void *log_ctx)
{
    AVExpr *e = NULL;
    int ret = av_expr_parse(&e, s, const_names, func1_names, funcs1, func2_names, funcs2, log_offset, log_ctx);
    if (ret < 0) {
        *d = NAN;
        return ret;
    }
    *d = av_expr_eval(e, const_values, opaque);
    av_expr_free(e);
    return isnan(*d) ? AVERROR(EINVAL) : 0;
}
