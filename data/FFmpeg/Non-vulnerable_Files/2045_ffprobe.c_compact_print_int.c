static void compact_print_int(WriterContext *wctx, const char *key, long long int value)
{
    CompactContext *compact = wctx->priv;
    if (wctx->nb_item[wctx->level]) printf("%c", compact->item_sep);
    if (!compact->nokey)
        printf("%s%s=", wctx->section_pbuf[wctx->level].str, key);
    printf("%lld", value);
}
