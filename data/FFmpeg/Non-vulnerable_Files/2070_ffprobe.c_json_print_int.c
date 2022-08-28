static void json_print_int(WriterContext *wctx, const char *key, long long int value)
{
    JSONContext *json = wctx->priv;
    AVBPrint buf;
    if (wctx->nb_item[wctx->level])
        printf("%s", json->item_sep);
    if (!json->compact)
        JSON_INDENT();
    av_bprint_init(&buf, 1, AV_BPRINT_SIZE_UNLIMITED);
    printf("\"%s\": %lld", json_escape_str(&buf, key, wctx), value);
    av_bprint_finalize(&buf, NULL);
}
