static inline int writer_print_string(WriterContext *wctx,
                                      const char *key, const char *val, int flags)
{
    const struct section *section = wctx->section[wctx->level];
    int ret = 0;
    if ((flags & PRINT_STRING_OPT)
        && !(wctx->writer->flags & WRITER_FLAG_DISPLAY_OPTIONAL_FIELDS))
        return 0;
    if (section->show_all_entries || av_dict_get(section->entries_to_show, key, NULL, 0)) {
        if (flags & PRINT_STRING_VALIDATE) {
            char *key1 = NULL, *val1 = NULL;
            ret = validate_string(wctx, &key1, key);
            if (ret < 0) goto end;
            ret = validate_string(wctx, &val1, val);
            if (ret < 0) goto end;
            wctx->writer->print_string(wctx, key1, val1);
        end:
            if (ret < 0) {
                av_log(wctx, AV_LOG_ERROR,
                       "Invalid key=value string combination %s=%s in section %s\n",
                       key, val, section->unique_name);
            }
            av_free(key1);
            av_free(val1);
        } else {
            wctx->writer->print_string(wctx, key, val);
        }
        wctx->nb_item[wctx->level]++;
    }
    return ret;
}
