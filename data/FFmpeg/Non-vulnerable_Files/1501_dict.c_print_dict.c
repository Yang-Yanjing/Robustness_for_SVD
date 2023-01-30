static void print_dict(const AVDictionary *m)
{
    AVDictionaryEntry *t = NULL;
    while ((t = av_dict_get(m, "", t, AV_DICT_IGNORE_SUFFIX)))
        printf("%s %s   ", t->key, t->value);
    printf("\n");
}
