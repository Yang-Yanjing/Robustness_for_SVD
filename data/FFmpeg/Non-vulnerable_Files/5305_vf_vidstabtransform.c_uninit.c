static av_cold void uninit(AVFilterContext *ctx)
{
    TransformContext *tc = ctx->priv;
    vsTransformDataCleanup(&tc->td);
    vsTransformationsCleanup(&tc->trans);
}
