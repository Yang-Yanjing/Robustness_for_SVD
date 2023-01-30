static void dilate_end_frame_filter(AVFilterContext *ctx, IplImage *inimg, IplImage *outimg)
{
    OCVContext *s = ctx->priv;
    DilateContext *dilate = s->priv;
    cvDilate(inimg, outimg, dilate->kernel, dilate->nb_iterations);
}
