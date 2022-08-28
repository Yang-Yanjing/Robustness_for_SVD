static av_cold int init_dict(AVFilterContext *ctx, AVDictionary **opts)
{
    AResampleContext *aresample = ctx->priv;
    int ret = 0;
    aresample->next_pts = AV_NOPTS_VALUE;
    aresample->swr = swr_alloc();
    if (!aresample->swr) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    if (opts) {
        AVDictionaryEntry *e = NULL;
        while ((e = av_dict_get(*opts, "", e, AV_DICT_IGNORE_SUFFIX))) {
            if ((ret = av_opt_set(aresample->swr, e->key, e->value, 0)) < 0)
                goto end;
        }
        av_dict_free(opts);
    }
    if (aresample->sample_rate_arg > 0)
        av_opt_set_int(aresample->swr, "osr", aresample->sample_rate_arg, 0);
end:
    return ret;
}
