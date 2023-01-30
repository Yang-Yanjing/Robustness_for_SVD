static int resample_linear(AVFilterContext *ctx, void *arg,
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
    for (y = start; y < end; y++){
        int sy = y << vsub;
        for (x = 0; x < w; x++){
            int u, v, subU, subV, sum, sx, index, subUI, subVI;
            sx   = x << hsub;
            u    = s->pv[sx + sy * linesize][0] >> hsub;
            v    = s->pv[sx + sy * linesize][1] >> vsub;
            subU = u & (SUB_PIXELS - 1);
            subV = v & (SUB_PIXELS - 1);
            u  >>= SUB_PIXEL_BITS;
            v  >>= SUB_PIXEL_BITS;
            index = u + v * src_linesize;
            subUI = SUB_PIXELS - subU;
            subVI = SUB_PIXELS - subV;
            if ((unsigned)u < (unsigned)(w - 1)){
                if((unsigned)v < (unsigned)(h - 1)){
                    sum = subVI * (subUI * src[index] + subU * src[index + 1]) +
                          subV  * (subUI * src[index + src_linesize] + subU * src[index + src_linesize + 1]);
                    sum = (sum + (1 << (SUB_PIXEL_BITS * 2 - 1)))>> (SUB_PIXEL_BITS * 2);
                } else {
                    if (v < 0)
                        v = 0;
                    else
                        v = h - 1;
                    index = u + v * src_linesize;
                    sum   = subUI * src[index] + subU * src[index + 1];
                    sum   = (sum + (1 << (SUB_PIXEL_BITS - 1))) >> SUB_PIXEL_BITS;
                }
            } else {
                if (u < 0)
                    u = 0;
                else
                    u = w - 1;
                if ((unsigned)v < (unsigned)(h - 1)){
                    index = u + v * src_linesize;
                    sum   = subVI * src[index] + subV * src[index + src_linesize];
                    sum   = (sum + (1 << (SUB_PIXEL_BITS - 1))) >> SUB_PIXEL_BITS;
                } else {
                    if (v < 0)
                        v = 0;
                    else
                        v = h - 1;
                    index = u + v * src_linesize;
                    sum   = src[index];
                }
            }
            sum = av_clip_uint8(sum);
            dst[x + y * dst_linesize] = sum;
        }
    }
    return 0;
}
