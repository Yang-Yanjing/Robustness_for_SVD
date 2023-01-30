static void smooth_end_frame_filter(AVFilterContext *ctx, IplImage *inimg, IplImage *outimg)
{
    OCVContext *s = ctx->priv;
    SmoothContext *smooth = s->priv;
    cvSmooth(inimg, outimg, smooth->type, smooth->param1, smooth->param2, smooth->param3, smooth->param4);
}
