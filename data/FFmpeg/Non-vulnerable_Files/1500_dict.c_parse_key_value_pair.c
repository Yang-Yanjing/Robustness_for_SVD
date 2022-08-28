static int parse_key_value_pair(AVDictionary **pm, const char **buf,
                                const char *key_val_sep, const char *pairs_sep,
                                int flags)
{
    char *key = av_get_token(buf, key_val_sep);
    char *val = NULL;
    int ret;
    if (key && *key && strspn(*buf, key_val_sep)) {
        (*buf)++;
        val = av_get_token(buf, pairs_sep);
    }
    if (key && *key && val && *val)
        ret = av_dict_set(pm, key, val, flags);
    else
        ret = AVERROR(EINVAL);
    av_freep(&key);
    av_freep(&val);
    return ret;
}
