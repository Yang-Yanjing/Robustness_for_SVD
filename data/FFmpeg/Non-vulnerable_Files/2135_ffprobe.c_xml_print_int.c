static void xml_print_int(WriterContext *wctx, const char *key, long long int value)
{
    if (wctx->nb_item[wctx->level])
        printf(" ");
    printf("%s=\"%lld\"", key, value);
}
