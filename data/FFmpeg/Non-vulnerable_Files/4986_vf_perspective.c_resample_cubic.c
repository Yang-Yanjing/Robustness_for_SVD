static int resample_cubic(AVFilterContext *ctx, void *arg,
                          int job, int nb_jobs)
{
    PerspectiveContext *s = ctx->priv;
    ThreadData *td = arg;
    uint8_t *dst = td->dst;
    int dst_linesize = td->dst_linesize;
    uint8_t *src = td->src;
    int src_linesize = td->src_linesize;
    int w = td->w;
    int h = td->h;
    int hsub = td->hsub;
    int vsub = td->vsub;
    int start = (h * job) / nb_jobs;
    int end   = (h * (job+1)) / nb_jobs;
    const int linesize = s->linesize[0];
    int x, y;
    for (y = start; y < end; y++) {
        int sy = y << vsub;
        for (x = 0; x < w; x++) {
            int u, v, subU, subV, sum, sx;
            sx   = x << hsub;
            u    = s->pv[sx + sy * linesize][0] >> hsub;
            v    = s->pv[sx + sy * linesize][1] >> vsub;
            subU = u & (SUB_PIXELS - 1);
            subV = v & (SUB_PIXELS - 1);
            u  >>= SUB_PIXEL_BITS;
            v  >>= SUB_PIXEL_BITS;
            if (u > 0 && v > 0 && u < w - 2 && v < h - 2){
                const int index = u + v*src_linesize;
                const int a = s->coeff[subU][0];
                const int b = s->coeff[subU][1];
                const int c = s->coeff[subU][2];
                const int d = s->coeff[subU][3];
                sum = s->coeff[subV][0] * (a * src[index - 1 -     src_linesize] + b * src[index - 0 -     src_linesize]  +
                                      c *      src[index + 1 -     src_linesize] + d * src[index + 2 -     src_linesize]) +
                      s->coeff[subV][1] * (a * src[index - 1                   ] + b * src[index - 0                   ]  +
                                      c *      src[index + 1                   ] + d * src[index + 2                   ]) +
                      s->coeff[subV][2] * (a * src[index - 1 +     src_linesize] + b * src[index - 0 +     src_linesize]  +
                                      c *      src[index + 1 +     src_linesize] + d * src[index + 2 +     src_linesize]) +
                      s->coeff[subV][3] * (a * src[index - 1 + 2 * src_linesize] + b * src[index - 0 + 2 * src_linesize]  +
                                      c *      src[index + 1 + 2 * src_linesize] + d * src[index + 2 + 2 * src_linesize]);
            } else {
                int dx, dy;
                sum = 0;
                for (dy = 0; dy < 4; dy++) {
                    int iy = v + dy - 1;
                    if (iy < 0)
                        iy = 0;
                    else if (iy >= h)
                        iy = h-1;
                    for (dx = 0; dx < 4; dx++) {
                        int ix = u + dx - 1;
                        if (ix < 0)
                            ix = 0;
                        else if (ix >= w)
                            ix = w - 1;
                        sum += s->coeff[subU][dx] * s->coeff[subV][dy] * src[ ix + iy * src_linesize];
                    }
                }
            }
            sum = (sum + (1<<(COEFF_BITS * 2 - 1))) >> (COEFF_BITS * 2);
            sum = av_clip_uint8(sum);
            dst[x + y * dst_linesize] = sum;
        }
    }
    return 0;
}
