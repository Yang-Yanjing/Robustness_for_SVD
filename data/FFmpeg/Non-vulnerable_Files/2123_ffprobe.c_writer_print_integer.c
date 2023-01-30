static inline void writer_print_integer(WriterContext *wctx,
                                        const char *key, long long int val)
{
    const struct section *section = wctx->section[wctx->level];
    if (section->show_all_entries || av_dict_get(section->entries_to_show, key, NULL, 0)) {
        wctx->writer->print_integer(wctx, key, val);
        wctx->nb_item[wctx->level]++;
    }
}
