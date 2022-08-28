int av_dict_parse_string(AVDictionary **pm, const char *str,
                         const char *key_val_sep, const char *pairs_sep,
                         int flags)
{
    int ret;
    if (!str)
        return 0;
    
    flags &= ~(AV_DICT_DONT_STRDUP_KEY | AV_DICT_DONT_STRDUP_VAL);
    while (*str) {
        if ((ret = parse_key_value_pair(pm, &str, key_val_sep, pairs_sep, flags)) < 0)
            return ret;
        if (*str)
            str++;
    }
    return 0;
}
