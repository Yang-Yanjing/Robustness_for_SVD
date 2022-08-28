void remove_avoptions(AVDictionary **a, AVDictionary *b)
{
    AVDictionaryEntry *t = NULL;
    while ((t = av_dict_get(b, "", t, AV_DICT_IGNORE_SUFFIX))) {
        av_dict_set(a, t->key, NULL, AV_DICT_MATCH_CASE);
    }
}
