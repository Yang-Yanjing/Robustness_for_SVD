static int filter_slice8(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    ATADenoiseContext *s = ctx->priv;
    ThreadData *td = arg;
    AVFrame *in = td->in;
    AVFrame *out = td->out;
    const int size = s->size;
    const int mid = s->mid;
    int p, x, y, i, j;
    for (p = 0; p < s->nb_planes; p++) {
        const int h = s->planeheight[p];
        const int w = s->planewidth[p];
        const int slice_start = (h * jobnr) / nb_jobs;
        const int slice_end = (h * (jobnr+1)) / nb_jobs;
        const uint8_t *src = in->data[p] + slice_start * in->linesize[p];
        uint8_t *dst = out->data[p] + slice_start * out->linesize[p];
        const int thra = s->thra[p];
        const int thrb = s->thrb[p];
        const uint8_t **data = (const uint8_t **)s->data[p];
        const int *linesize = (const int *)s->linesize[p];
        const uint8_t *srcf[SIZE];
        for (i = 0; i < size; i++)
            srcf[i] = data[i] + slice_start * linesize[i];
        for (y = slice_start; y < slice_end; y++) {
            for (x = 0; x < w; x++) {
                const int srcx = src[x];
                unsigned lsumdiff = 0, rsumdiff = 0;
                unsigned ldiff, rdiff;
                unsigned sum = srcx;
                int l = 0, r = 0;
                int srcjx, srcix;
                for (j = mid - 1, i = mid + 1; j >= 0 && i < size; j--, i++) {
                    srcjx = srcf[j][x];
                    ldiff = FFABS(srcx - srcjx);
                    lsumdiff += ldiff;
                    if (ldiff > thra ||
                        lsumdiff > thrb)
                        break;
                    l++;
                    sum += srcjx;
                    srcix = srcf[i][x];
                    rdiff = FFABS(srcx - srcix);
                    rsumdiff += rdiff;
                    if (rdiff > thra ||
                        rsumdiff > thrb)
                        break;
                    r++;
                    sum += srcix;
                }
                dst[x] = sum / (r + l + 1);
            }
            dst += out->linesize[p];
            src += in->linesize[p];
            for (i = 0; i < size; i++)
                srcf[i] += linesize[i];
        }
    }
    return 0;
}
