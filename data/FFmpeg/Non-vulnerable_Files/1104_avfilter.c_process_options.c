static int process_options(AVFilterContext *ctx, AVDictionary **options,
                           const char *args)
{
    const AVOption *o = NULL;
    int ret, count = 0;
    char *av_uninit(parsed_key), *av_uninit(value);
    const char *key;
    int offset= -1;
    if (!args)
        return 0;
    while (*args) {
        const char *shorthand = NULL;
        o = av_opt_next(ctx->priv, o);
        if (o) {
            if (o->type == AV_OPT_TYPE_CONST || o->offset == offset)
                continue;
            offset = o->offset;
            shorthand = o->name;
        }
        ret = av_opt_get_key_value(&args, "=", ":",
                                   shorthand ? AV_OPT_FLAG_IMPLICIT_KEY : 0,
                                   &parsed_key, &value);
        if (ret < 0) {
            if (ret == AVERROR(EINVAL))
                av_log(ctx, AV_LOG_ERROR, "No option name near '%s'\n", args);
            else
                av_log(ctx, AV_LOG_ERROR, "Unable to parse '%s': %s\n", args,
                       av_err2str(ret));
            return ret;
        }
        if (*args)
            args++;
        if (parsed_key) {
            key = parsed_key;
            while ((o = av_opt_next(ctx->priv, o))); 
        } else {
            key = shorthand;
        }
        av_log(ctx, AV_LOG_DEBUG, "Setting '%s' to value '%s'\n", key, value);
        if (av_opt_find(ctx, key, NULL, 0, 0)) {
            ret = av_opt_set(ctx, key, value, 0);
            if (ret < 0) {
                av_free(value);
                av_free(parsed_key);
                return ret;
            }
        } else {
        av_dict_set(options, key, value, 0);
        if ((ret = av_opt_set(ctx->priv, key, value, 0)) < 0) {
            if (!av_opt_find(ctx->priv, key, NULL, 0, AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ)) {
            if (ret == AVERROR_OPTION_NOT_FOUND)
                av_log(ctx, AV_LOG_ERROR, "Option '%s' not found\n", key);
            av_free(value);
            av_free(parsed_key);
            return ret;
            }
        }
        }
        av_free(value);
        av_free(parsed_key);
        count++;
    }
    if (ctx->enable_str) {
        ret = set_enable_expr(ctx, ctx->enable_str);
        if (ret < 0)
            return ret;
    }
    return count;
}
