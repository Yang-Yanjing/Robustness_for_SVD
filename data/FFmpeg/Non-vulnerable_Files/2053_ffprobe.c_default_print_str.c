static void default_print_str(WriterContext *wctx, const char *key, const char *value)
{
    DefaultContext *def = wctx->priv;
    if (!def->nokey)
        printf("%s%s=", wctx->section_pbuf[wctx->level].str, key);
    printf("%s\n", value);
}
