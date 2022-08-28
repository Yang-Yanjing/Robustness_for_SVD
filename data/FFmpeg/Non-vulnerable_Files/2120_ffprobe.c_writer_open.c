static int writer_open(WriterContext **wctx, const Writer *writer, const char *args,
                       const struct section *sections, int nb_sections)
{
    int i, ret = 0;
    if (!(*wctx = av_mallocz(sizeof(WriterContext)))) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    if (!((*wctx)->priv = av_mallocz(writer->priv_size))) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    (*wctx)->class = &writer_class;
    (*wctx)->writer = writer;
    (*wctx)->level = -1;
    (*wctx)->sections = sections;
    (*wctx)->nb_sections = nb_sections;
    av_opt_set_defaults(*wctx);
    if (writer->priv_class) {
        void *priv_ctx = (*wctx)->priv;
        *((const AVClass **)priv_ctx) = writer->priv_class;
        av_opt_set_defaults(priv_ctx);
    }
    
    if (args) {
        AVDictionary *opts = NULL;
        AVDictionaryEntry *opt = NULL;
        if ((ret = av_dict_parse_string(&opts, args, "=", ":", 0)) < 0) {
            av_log(*wctx, AV_LOG_ERROR, "Failed to parse option string '%s' provided to writer context\n", args);
            av_dict_free(&opts);
            goto fail;
        }
        while ((opt = av_dict_get(opts, "", opt, AV_DICT_IGNORE_SUFFIX))) {
            if ((ret = av_opt_set(*wctx, opt->key, opt->value, AV_OPT_SEARCH_CHILDREN)) < 0) {
                av_log(*wctx, AV_LOG_ERROR, "Failed to set option '%s' with value '%s' provided to writer context\n",
                       opt->key, opt->value);
                av_dict_free(&opts);
                goto fail;
            }
        }
        av_dict_free(&opts);
    }
    
    {
        const uint8_t *p = (*wctx)->string_validation_replacement;
        const uint8_t *endp = p + strlen(p);
        while (*p) {
            const uint8_t *p0 = p;
            int32_t code;
            ret = av_utf8_decode(&code, &p, endp, (*wctx)->string_validation_utf8_flags);
            if (ret < 0) {
                AVBPrint bp;
                av_bprint_init(&bp, 0, AV_BPRINT_SIZE_AUTOMATIC);
                bprint_bytes(&bp, p0, p-p0),
                    av_log(wctx, AV_LOG_ERROR,
                           "Invalid UTF8 sequence %s found in string validation replace '%s'\n",
                           bp.str, (*wctx)->string_validation_replacement);
                return ret;
            }
        }
    }
    for (i = 0; i < SECTION_MAX_NB_LEVELS; i++)
        av_bprint_init(&(*wctx)->section_pbuf[i], 1, AV_BPRINT_SIZE_UNLIMITED);
    if ((*wctx)->writer->init)
        ret = (*wctx)->writer->init(*wctx);
    if (ret < 0)
        goto fail;
    return 0;
fail:
    writer_close(wctx);
    return ret;
}
