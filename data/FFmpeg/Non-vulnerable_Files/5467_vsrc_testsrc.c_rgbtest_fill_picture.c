static void rgbtest_fill_picture(AVFilterContext *ctx, AVFrame *frame)
{
    TestSourceContext *test = ctx->priv;
    int x, y, w = frame->width, h = frame->height;
    for (y = 0; y < h; y++) {
         for (x = 0; x < w; x++) {
             int c = 256*x/w;
             int r = 0, g = 0, b = 0;
             if      (3*y < h  ) r = c;
             else if (3*y < 2*h) g = c;
             else                b = c;
             rgbtest_put_pixel(frame->data[0], frame->linesize[0], x, y, r, g, b,
                               ctx->outputs[0]->format, test->rgba_map);
         }
     }
}
