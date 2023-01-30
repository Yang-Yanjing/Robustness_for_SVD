static int filter_slice(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    RemoveGrainContext *s = ctx->priv;
    ThreadData *td = arg;
    AVFrame *in = td->in;
    AVFrame *out = td->out;
    const int i = td->plane;
    const int height = s->planeheight[i];
    const int om = in->linesize[i] - 1;
    const int o0 = in->linesize[i]    ;
    const int op = in->linesize[i] + 1;
    int start = (height *  jobnr   ) / nb_jobs;
    int end   = (height * (jobnr+1)) / nb_jobs;
    int x, y;
    start = FFMAX(1, start);
    end   = FFMIN(height-1, end);
    for (y = start; y < end; y++) {
        uint8_t *dst = out->data[i];
        uint8_t *src = in->data[i];
        src = in->data[i] + y * in->linesize[i];
        dst = out->data[i] + y * out->linesize[i];
        if (s->skip_even && !(y & 1)) {
            memcpy(dst, src, s->planewidth[i]);
            continue;
        }
        if (s->skip_odd && y & 1) {
            memcpy(dst, src, s->planewidth[i]);
            continue;
        }
        *dst++ = *src++;
        if (s->fl[i]) {
            int w_asm = (s->planewidth[i] - 2) & ~15;
            s->fl[i](dst, src, in->linesize[i], w_asm);
            x = 1 + w_asm;
            dst += w_asm;
            src += w_asm;
        } else
            x = 1;
        for (; x < s->planewidth[i] - 1; x++) {
            const int a1 = src[-op];
            const int a2 = src[-o0];
            const int a3 = src[-om];
            const int a4 = src[-1 ];
            const int c  = src[ 0 ];
            const int a5 = src[ 1 ];
            const int a6 = src[ om];
            const int a7 = src[ o0];
            const int a8 = src[ op];
            const int res = s->rg[i](c, a1, a2, a3, a4, a5, a6, a7, a8);
            *dst = res;
            dst++, src++;
        }
        dst[0] = src[0];
    }
    return 0;
}
