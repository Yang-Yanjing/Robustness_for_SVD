static int func_metadata(AVFilterContext *ctx, AVBPrint *bp,
                         char *fct, unsigned argc, char **argv, int tag)
{
    DrawTextContext *s = ctx->priv;
    AVDictionaryEntry *e = av_dict_get(s->metadata, argv[0], NULL, 0);
    if (e && e->value)
        av_bprintf(bp, "%s", e->value);
    return 0;
}
