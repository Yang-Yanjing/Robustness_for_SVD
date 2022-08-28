static int func_eval_expr_int_format(AVFilterContext *ctx, AVBPrint *bp,
                          char *fct, unsigned argc, char **argv, int tag)
{
    DrawTextContext *s = ctx->priv;
    double res;
    int intval;
    int ret;
    unsigned int positions = 0;
    char fmt_str[30] = "%";
    




    ret = av_expr_parse_and_eval(&res, argv[0], var_names, s->var_values,
                                 NULL, NULL, fun2_names, fun2,
                                 &s->prng, 0, ctx);
    if (ret < 0) {
        av_log(ctx, AV_LOG_ERROR,
               "Expression '%s' for the expr text expansion function is not valid\n",
               argv[0]);
        return ret;
    }
    if (!strchr("xXdu", argv[1][0])) {
        av_log(ctx, AV_LOG_ERROR, "Invalid format '%c' specified,"
                " allowed values: 'x', 'X', 'd', 'u'\n", argv[1][0]);
        return AVERROR(EINVAL);
    }
    if (argc == 3) {
        ret = sscanf(argv[2], "%u", &positions);
        if (ret != 1) {
            av_log(ctx, AV_LOG_ERROR, "expr_int_format(): Invalid number of positions"
                    " to print: '%s'\n", argv[2]);
            return AVERROR(EINVAL);
        }
    }
    feclearexcept(FE_ALL_EXCEPT);
    intval = res;
    if ((ret = fetestexcept(FE_INVALID|FE_OVERFLOW|FE_UNDERFLOW))) {
        av_log(ctx, AV_LOG_ERROR, "Conversion of floating-point result to int failed. Control register: 0x%08x. Conversion result: %d\n", ret, intval);
        return AVERROR(EINVAL);
    }
    if (argc == 3)
        av_strlcatf(fmt_str, sizeof(fmt_str), "0%u", positions);
    av_strlcatf(fmt_str, sizeof(fmt_str), "%c", argv[1][0]);
    av_log(ctx, AV_LOG_DEBUG, "Formatting value %f (expr '%s') with spec '%s'\n",
            res, argv[0], fmt_str);
    av_bprintf(bp, fmt_str, intval);
    return 0;
}
