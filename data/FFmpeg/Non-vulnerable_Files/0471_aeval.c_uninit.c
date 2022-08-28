static av_cold void uninit(AVFilterContext *ctx)
{
    EvalContext *eval = ctx->priv;
    int i;
    for (i = 0; i < eval->nb_channels; i++) {
        av_expr_free(eval->expr[i]);
        eval->expr[i] = NULL;
    }
    av_freep(&eval->expr);
    av_freep(&eval->channel_values);
}
