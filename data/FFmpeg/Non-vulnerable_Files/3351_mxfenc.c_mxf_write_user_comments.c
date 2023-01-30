static int mxf_write_user_comments(AVFormatContext *s, const AVDictionary *m)
{
    MXFContext *mxf = s->priv_data;
    AVDictionaryEntry *t = NULL;
    int count = 0;
    while ((t = av_dict_get(m, "comment_", t, AV_DICT_IGNORE_SUFFIX))) {
        if (mxf->tagged_value_count >= UINT16_MAX) {
            av_log(s, AV_LOG_ERROR, "too many tagged values, ignoring remaining\n");
            return count;
        }
        if (mxf_write_tagged_value(s, t->key + 8, t->value) == 0)
            count++;
    }
    return count;
}
