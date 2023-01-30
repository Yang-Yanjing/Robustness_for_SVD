static av_cold int init_dict(AVFilterContext *ctx, AVDictionary **opts)
{
    SPPContext *s = ctx->priv;
    int ret;
    s->avctx = avcodec_alloc_context3(NULL);
    s->dct = avcodec_dct_alloc();
    if (!s->avctx || !s->dct)
        return AVERROR(ENOMEM);
    if (opts) {
        AVDictionaryEntry *e = NULL;
        while ((e = av_dict_get(*opts, "", e, AV_DICT_IGNORE_SUFFIX))) {
            if ((ret = av_opt_set(s->dct, e->key, e->value, 0)) < 0)
                return ret;
        }
        av_dict_free(opts);
    }
    s->store_slice = store_slice_c;
    switch (s->mode) {
    case MODE_HARD: s->requantize = hardthresh_c; break;
    case MODE_SOFT: s->requantize = softthresh_c; break;
    }
    return 0;
}
