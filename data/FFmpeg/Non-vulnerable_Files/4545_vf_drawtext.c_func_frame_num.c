static int func_frame_num(AVFilterContext *ctx, AVBPrint *bp,
                          char *fct, unsigned argc, char **argv, int tag)
{
    DrawTextContext *s = ctx->priv;
    av_bprintf(bp, "%d", (int)s->var_values[VAR_N]);
    return 0;
}
