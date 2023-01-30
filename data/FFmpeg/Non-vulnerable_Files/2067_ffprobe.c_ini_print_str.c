static void ini_print_str(WriterContext *wctx, const char *key, const char *value)
{
    AVBPrint buf;
    av_bprint_init(&buf, 1, AV_BPRINT_SIZE_UNLIMITED);
    printf("%s=", ini_escape_str(&buf, key));
    av_bprint_clear(&buf);
    printf("%s\n", ini_escape_str(&buf, value));
    av_bprint_finalize(&buf, NULL);
}
