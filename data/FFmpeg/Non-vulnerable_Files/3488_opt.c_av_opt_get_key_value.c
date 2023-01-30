int av_opt_get_key_value(const char **ropts,
                         const char *key_val_sep, const char *pairs_sep,
                         unsigned flags,
                         char **rkey, char **rval)
{
    int ret;
    char *key = NULL, *val;
    const char *opts = *ropts;
    if ((ret = get_key(&opts, key_val_sep, &key)) < 0 &&
        !(flags & AV_OPT_FLAG_IMPLICIT_KEY))
        return AVERROR(EINVAL);
    if (!(val = av_get_token(&opts, pairs_sep))) {
        av_free(key);
        return AVERROR(ENOMEM);
    }
    *ropts = opts;
    *rkey  = key;
    *rval  = val;
    return 0;
}
