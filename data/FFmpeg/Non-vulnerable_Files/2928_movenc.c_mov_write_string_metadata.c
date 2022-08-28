static int mov_write_string_metadata(AVFormatContext *s, AVIOContext *pb,
                                     const char *name, const char *tag,
                                     int long_style)
{
    int l, lang = 0, len, len2;
    AVDictionaryEntry *t, *t2 = NULL;
    char tag2[16];
    if (!(t = av_dict_get(s->metadata, tag, NULL, 0)))
        return 0;
    len = strlen(t->key);
    snprintf(tag2, sizeof(tag2), "%s-", tag);
    while ((t2 = av_dict_get(s->metadata, tag2, t2, AV_DICT_IGNORE_SUFFIX))) {
        len2 = strlen(t2->key);
        if (len2 == len + 4 && !strcmp(t->value, t2->value)
            && (l = ff_mov_iso639_to_lang(&t2->key[len2 - 3], 1)) >= 0) {
            lang = l;
            break;
        }
    }
    return mov_write_string_tag(pb, name, t->value, lang, long_style);
}
