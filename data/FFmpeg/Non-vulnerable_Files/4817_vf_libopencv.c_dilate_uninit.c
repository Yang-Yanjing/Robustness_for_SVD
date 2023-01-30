static av_cold void dilate_uninit(AVFilterContext *ctx)
{
    OCVContext *s = ctx->priv;
    DilateContext *dilate = s->priv;
    cvReleaseStructuringElement(&dilate->kernel);
}
