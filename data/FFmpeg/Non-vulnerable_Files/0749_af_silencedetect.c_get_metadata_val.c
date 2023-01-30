AVFILTER_DEFINE_CLASS(silencedetect);
static char *get_metadata_val(AVFrame *insamples, const char *key)
{
    AVDictionaryEntry *e = av_dict_get(insamples->metadata, key, NULL, 0);
    return e && e->value ? e->value : NULL;
}
