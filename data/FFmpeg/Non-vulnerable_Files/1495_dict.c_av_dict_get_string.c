int av_dict_get_string(const AVDictionary *m, char **buffer,
                       const char key_val_sep, const char pairs_sep)
{
    AVDictionaryEntry *t = NULL;
    AVBPrint bprint;
    int cnt = 0;
    char special_chars[] = {pairs_sep, key_val_sep, '\0'};
    if (!buffer || pairs_sep == '\0' || key_val_sep == '\0' || pairs_sep == key_val_sep ||
        pairs_sep == '\\' || key_val_sep == '\\')
        return AVERROR(EINVAL);
    if (!av_dict_count(m)) {
        *buffer = av_strdup("");
        return *buffer ? 0 : AVERROR(ENOMEM);
    }
    av_bprint_init(&bprint, 64, AV_BPRINT_SIZE_UNLIMITED);
    while ((t = av_dict_get(m, "", t, AV_DICT_IGNORE_SUFFIX))) {
        if (cnt++)
            av_bprint_append_data(&bprint, &pairs_sep, 1);
        av_bprint_escape(&bprint, t->key, special_chars, AV_ESCAPE_MODE_BACKSLASH, 0);
        av_bprint_append_data(&bprint, &key_val_sep, 1);
        av_bprint_escape(&bprint, t->value, special_chars, AV_ESCAPE_MODE_BACKSLASH, 0);
    }
    return av_bprint_finalize(&bprint, buffer);
}
