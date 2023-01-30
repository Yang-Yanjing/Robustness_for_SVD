static av_cold void geq_uninit(AVFilterContext *ctx)
{
    int i;
    GEQContext *geq = ctx->priv;
    for (i = 0; i < FF_ARRAY_ELEMS(geq->e); i++)
        av_expr_free(geq->e[i]);
}
