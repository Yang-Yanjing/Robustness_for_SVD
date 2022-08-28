static inline void json_print_item_str(WriterContext *wctx,
                                       const char *key, const char *value)
{
    AVBPrint buf;
    av_bprint_init(&buf, 1, AV_BPRINT_SIZE_UNLIMITED);
    printf("\"%s\":", json_escape_str(&buf, key,   wctx));
    av_bprint_clear(&buf);
    printf(" \"%s\"", json_escape_str(&buf, value, wctx));
    av_bprint_finalize(&buf, NULL);
}
