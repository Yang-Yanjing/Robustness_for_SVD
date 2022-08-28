static int func_pict_type(AVFilterContext *ctx, AVBPrint *bp,
                          char *fct, unsigned argc, char **argv, int tag)
{
    DrawTextContext *s = ctx->priv;
    av_bprintf(bp, "%c", av_get_picture_type_char(s->var_values[VAR_PICT_TYPE]));
    return 0;
}
