static int diff_planes(AVFilterContext *ctx,
                       uint8_t *cur, int cur_linesize,
                       uint8_t *ref, int ref_linesize,
                       int w, int h)
{
    DecimateContext *decimate = ctx->priv;
    int x, y;
    int d, c = 0;
    int t = (w/16)*(h/16)*decimate->frac;
    
    for (y = 0; y < h-7; y += 4) {
        for (x = 8; x < w-7; x += 4) {
            d = decimate->sad(cur + y*cur_linesize + x, cur_linesize,
                              ref + y*ref_linesize + x, ref_linesize);
            if (d > decimate->hi) {
                av_log(ctx, AV_LOG_DEBUG, "%d>=hi ", d);
                return 1;
            }
            if (d > decimate->lo) {
                c++;
                if (c > t) {
                    av_log(ctx, AV_LOG_DEBUG, "lo:%d>=%d ", c, t);
                    return 1;
                }
            }
        }
    }
    av_log(ctx, AV_LOG_DEBUG, "lo:%d<%d ", c, t);
    return 0;
}
