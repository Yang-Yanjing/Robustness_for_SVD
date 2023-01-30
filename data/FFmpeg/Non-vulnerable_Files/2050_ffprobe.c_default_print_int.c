static void default_print_int(WriterContext *wctx, const char *key, long long int value)
{
    DefaultContext *def = wctx->priv;
    if (!def->nokey)
        printf("%s%s=", wctx->section_pbuf[wctx->level].str, key);
    printf("%lld\n", value);
}
