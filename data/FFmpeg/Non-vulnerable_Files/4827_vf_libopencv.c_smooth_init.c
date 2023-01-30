static av_cold int smooth_init(AVFilterContext *ctx, const char *args)
{
    OCVContext *s = ctx->priv;
    SmoothContext *smooth = s->priv;
    char type_str[128] = "gaussian";
    smooth->param1 = 3;
    smooth->param2 = 0;
    smooth->param3 = 0.0;
    smooth->param4 = 0.0;
    if (args)
        sscanf(args, "%127[^|]|%d|%d|%lf|%lf", type_str, &smooth->param1, &smooth->param2, &smooth->param3, &smooth->param4);
    if      (!strcmp(type_str, "blur"         )) smooth->type = CV_BLUR;
    else if (!strcmp(type_str, "blur_no_scale")) smooth->type = CV_BLUR_NO_SCALE;
    else if (!strcmp(type_str, "median"       )) smooth->type = CV_MEDIAN;
    else if (!strcmp(type_str, "gaussian"     )) smooth->type = CV_GAUSSIAN;
    else if (!strcmp(type_str, "bilateral"    )) smooth->type = CV_BILATERAL;
    else {
        av_log(ctx, AV_LOG_ERROR, "Smoothing type '%s' unknown.\n", type_str);
        return AVERROR(EINVAL);
    }
    if (smooth->param1 < 0 || !(smooth->param1%2)) {
        av_log(ctx, AV_LOG_ERROR,
               "Invalid value '%d' for param1, it has to be a positive odd number\n",
               smooth->param1);
        return AVERROR(EINVAL);
    }
    if ((smooth->type == CV_BLUR || smooth->type == CV_BLUR_NO_SCALE || smooth->type == CV_GAUSSIAN) &&
        (smooth->param2 < 0 || (smooth->param2 && !(smooth->param2%2)))) {
        av_log(ctx, AV_LOG_ERROR,
               "Invalid value '%d' for param2, it has to be zero or a positive odd number\n",
               smooth->param2);
        return AVERROR(EINVAL);
    }
    av_log(ctx, AV_LOG_VERBOSE, "type:%s param1:%d param2:%d param3:%f param4:%f\n",
           type_str, smooth->param1, smooth->param2, smooth->param3, smooth->param4);
    return 0;
}
