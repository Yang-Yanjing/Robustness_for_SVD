int av_opt_set_from_string(void *ctx, const char *opts,
                           const char *const *shorthand,
                           const char *key_val_sep, const char *pairs_sep)
{
    int ret, count = 0;
    const char *dummy_shorthand = NULL;
    char *av_uninit(parsed_key), *av_uninit(value);
    const char *key;
    if (!opts)
        return 0;
    if (!shorthand)
        shorthand = &dummy_shorthand;
    while (*opts) {
        ret = av_opt_get_key_value(&opts, key_val_sep, pairs_sep,
                                   *shorthand ? AV_OPT_FLAG_IMPLICIT_KEY : 0,
                                   &parsed_key, &value);
        if (ret < 0) {
            if (ret == AVERROR(EINVAL))
                av_log(ctx, AV_LOG_ERROR, "No option name near '%s'\n", opts);
            else
                av_log(ctx, AV_LOG_ERROR, "Unable to parse '%s': %s\n", opts,
                       av_err2str(ret));
            return ret;
        }
        if (*opts)
            opts++;
        if (parsed_key) {
            key = parsed_key;
            while (*shorthand) 
                shorthand++;
        } else {
            key = *(shorthand++);
        }
        av_log(ctx, AV_LOG_DEBUG, "Setting '%s' to value '%s'\n", key, value);
        if ((ret = av_opt_set(ctx, key, value, 0)) < 0) {
            if (ret == AVERROR_OPTION_NOT_FOUND)
                av_log(ctx, AV_LOG_ERROR, "Option '%s' not found\n", key);
            av_free(value);
            av_free(parsed_key);
            return ret;
        }
        av_free(value);
        av_free(parsed_key);
        count++;
    }
    return count;
}
