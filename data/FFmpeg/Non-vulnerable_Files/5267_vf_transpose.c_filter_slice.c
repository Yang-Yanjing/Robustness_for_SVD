static int filter_slice(AVFilterContext *ctx, void *arg, int jobnr,
                        int nb_jobs)
{
    TransContext *trans = ctx->priv;
    ThreadData *td = arg;
    AVFrame *out = td->out;
    AVFrame *in = td->in;
    int plane;
    for (plane = 0; out->data[plane]; plane++) {
        int hsub    = plane == 1 || plane == 2 ? trans->hsub : 0;
        int vsub    = plane == 1 || plane == 2 ? trans->vsub : 0;
        int pixstep = trans->pixsteps[plane];
        int inh     = FF_CEIL_RSHIFT(in->height, vsub);
        int outw    = FF_CEIL_RSHIFT(out->width,  hsub);
        int outh    = FF_CEIL_RSHIFT(out->height, vsub);
        int start   = (outh *  jobnr   ) / nb_jobs;
        int end     = (outh * (jobnr+1)) / nb_jobs;
        uint8_t *dst, *src;
        int dstlinesize, srclinesize;
        int x, y;
        dstlinesize = out->linesize[plane];
        dst         = out->data[plane] + start * dstlinesize;
        src         = in->data[plane];
        srclinesize = in->linesize[plane];
        if (trans->dir & 1) {
            src         += in->linesize[plane] * (inh - 1);
            srclinesize *= -1;
        }
        if (trans->dir & 2) {
            dst          = out->data[plane] + dstlinesize * (outh - start - 1);
            dstlinesize *= -1;
        }
        switch (pixstep) {
        case 1:
            for (y = start; y < end; y++, dst += dstlinesize)
                for (x = 0; x < outw; x++)
                    dst[x] = src[x * srclinesize + y];
            break;
        case 2:
            for (y = start; y < end; y++, dst += dstlinesize) {
                for (x = 0; x < outw; x++)
                    *((uint16_t *)(dst + 2 * x)) =
                        *((uint16_t *)(src + x * srclinesize + y * 2));
            }
            break;
        case 3:
            for (y = start; y < end; y++, dst += dstlinesize) {
                for (x = 0; x < outw; x++) {
                    int32_t v = AV_RB24(src + x * srclinesize + y * 3);
                    AV_WB24(dst + 3 * x, v);
                }
            }
            break;
        case 4:
            for (y = start; y < end; y++, dst += dstlinesize) {
                for (x = 0; x < outw; x++)
                    *((uint32_t *)(dst + 4 * x)) =
                        *((uint32_t *)(src + x * srclinesize + y * 4));
            }
            break;
        case 6:
            for (y = start; y < end; y++, dst += dstlinesize) {
                for (x = 0; x < outw; x++) {
                    int64_t v = AV_RB48(src + x * srclinesize + y*6);
                    AV_WB48(dst + 6*x, v);
                }
            }
            break;
        case 8:
            for (y = start; y < end; y++, dst += dstlinesize) {
                for (x = 0; x < outw; x++)
                    *((uint64_t *)(dst + 8*x)) = *((uint64_t *)(src + x * srclinesize + y*8));
            }
            break;
        }
    }
    return 0;
}
