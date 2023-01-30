static void erode_end_frame_filter(AVFilterContext *ctx, IplImage *inimg, IplImage *outimg)
{
    OCVContext *s = ctx->priv;
    DilateContext *dilate = s->priv;
    cvErode(inimg, outimg, dilate->kernel, dilate->nb_iterations);
}
