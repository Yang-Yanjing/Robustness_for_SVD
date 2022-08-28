static void json_print_str(WriterContext *wctx, const char *key, const char *value)
{
    JSONContext *json = wctx->priv;
    if (wctx->nb_item[wctx->level])
        printf("%s", json->item_sep);
    if (!json->compact)
        JSON_INDENT();
    json_print_item_str(wctx, key, value);
}
