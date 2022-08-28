AVFILTER_DEFINE_CLASS(geq);
static inline double getpix(void *priv, double x, double y, int plane)
{
    int xi, yi;
    GEQContext *geq = priv;
    AVFrame *picref = geq->picref;
    const uint8_t *src = picref->data[plane];
    const int linesize = picref->linesize[plane];
    const int w = (plane == 1 || plane == 2) ? FF_CEIL_RSHIFT(picref->width,  geq->hsub) : picref->width;
    const int h = (plane == 1 || plane == 2) ? FF_CEIL_RSHIFT(picref->height, geq->vsub) : picref->height;
    if (!src)
        return 0;
    xi = x = av_clipf(x, 0, w - 2);
    yi = y = av_clipf(y, 0, h - 2);
    x -= xi;
    y -= yi;
    return (1-y)*((1-x)*src[xi +  yi    * linesize] + x*src[xi + 1 +  yi    * linesize])
          +   y *((1-x)*src[xi + (yi+1) * linesize] + x*src[xi + 1 + (yi+1) * linesize]);
}
