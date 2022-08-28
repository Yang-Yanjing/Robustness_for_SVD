static void flat_print_int(WriterContext *wctx, const char *key, long long int value)
{
    printf("%s%s=%lld\n", wctx->section_pbuf[wctx->level].str, key, value);
}
