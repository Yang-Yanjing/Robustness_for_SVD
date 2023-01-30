static int filter_slice(AVFilterContext *ctx, void *arg, int job, int nb_jobs)
{
    FlipContext *s = ctx->priv;
    ThreadData *td = arg;
    AVFrame *in = td->in;
    AVFrame *out = td->out;
    uint8_t *inrow, *outrow;
    int i, j, plane, step;
    for (plane = 0; plane < 4 && in->data[plane] && in->linesize[plane]; plane++) {
        const int width  = s->planewidth[plane];
        const int height = s->planeheight[plane];
        const int start = (height *  job   ) / nb_jobs;
        const int end   = (height * (job+1)) / nb_jobs;
        step = s->max_step[plane];
        outrow = out->data[plane] + start * out->linesize[plane];
        inrow  = in ->data[plane] + start * in->linesize[plane] + (width - 1) * step;
        for (i = start; i < end; i++) {
            switch (step) {
            case 1:
                for (j = 0; j < width; j++)
                    outrow[j] = inrow[-j];
            break;
            case 2:
            {
                uint16_t *outrow16 = (uint16_t *)outrow;
                uint16_t * inrow16 = (uint16_t *) inrow;
                for (j = 0; j < width; j++)
                    outrow16[j] = inrow16[-j];
            }
            break;
            case 3:
            {
                uint8_t *in  =  inrow;
                uint8_t *out = outrow;
                for (j = 0; j < width; j++, out += 3, in -= 3) {
                    int32_t v = AV_RB24(in);
                    AV_WB24(out, v);
                }
            }
            break;
            case 4:
            {
                uint32_t *outrow32 = (uint32_t *)outrow;
                uint32_t * inrow32 = (uint32_t *) inrow;
                for (j = 0; j < width; j++)
                    outrow32[j] = inrow32[-j];
            }
            break;
            default:
                for (j = 0; j < width; j++)
                    memcpy(outrow + j*step, inrow - j*step, step);
            }
            inrow  += in ->linesize[plane];
            outrow += out->linesize[plane];
        }
    }
    return 0;
}
