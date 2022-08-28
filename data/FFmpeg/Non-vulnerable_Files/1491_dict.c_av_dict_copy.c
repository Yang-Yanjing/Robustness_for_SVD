void av_dict_copy(AVDictionary **dst, const AVDictionary *src, int flags)
{
    AVDictionaryEntry *t = NULL;
    while ((t = av_dict_get(src, "", t, AV_DICT_IGNORE_SUFFIX)))
        av_dict_set(dst, t->key, t->value, flags);
}
