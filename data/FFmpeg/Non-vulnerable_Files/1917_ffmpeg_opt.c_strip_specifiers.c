static AVDictionary *strip_specifiers(AVDictionary *dict)
{
    AVDictionaryEntry *e = NULL;
    AVDictionary    *ret = NULL;
    while ((e = av_dict_get(dict, "", e, AV_DICT_IGNORE_SUFFIX))) {
        char *p = strchr(e->key, ':');
        if (p)
            *p = 0;
        av_dict_set(&ret, e->key, e->value, 0);
        if (p)
            *p = ':';
    }
    return ret;
}
