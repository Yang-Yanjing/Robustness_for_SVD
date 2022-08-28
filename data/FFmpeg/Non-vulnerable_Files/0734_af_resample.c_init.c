static av_cold int init(AVFilterContext *ctx, AVDictionary **opts)
{
    ResampleContext *s = ctx->priv;
    const AVClass *avr_class = avresample_get_class();
    AVDictionaryEntry *e = NULL;
    while ((e = av_dict_get(*opts, "", e, AV_DICT_IGNORE_SUFFIX))) {
        if (av_opt_find(&avr_class, e->key, NULL, 0,
                        AV_OPT_SEARCH_FAKE_OBJ | AV_OPT_SEARCH_CHILDREN))
            av_dict_set(&s->options, e->key, e->value, 0);
    }
    e = NULL;
    while ((e = av_dict_get(s->options, "", e, AV_DICT_IGNORE_SUFFIX)))
        av_dict_set(opts, e->key, NULL, 0);
    
    av_dict_set(&s->options,  "in_channel_layout", NULL, 0);
    av_dict_set(&s->options, "out_channel_layout", NULL, 0);
    av_dict_set(&s->options,  "in_sample_fmt",     NULL, 0);
    av_dict_set(&s->options, "out_sample_fmt",     NULL, 0);
    av_dict_set(&s->options,  "in_sample_rate",    NULL, 0);
    av_dict_set(&s->options, "out_sample_rate",    NULL, 0);
    return 0;
}
