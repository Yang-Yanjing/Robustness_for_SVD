static int process_slice_uyvy422(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    const ThreadData *td = arg;
    const AVFrame *src = td->src;
    AVFrame *dst = td->dst;
    const int height = src->height;
    const int width = src->width*2;
    const int src_pitch = src->linesize[0];
    const int dst_pitch = dst->linesize[0];
    const int slice_start = (height *  jobnr   ) / nb_jobs;
    const int slice_end   = (height * (jobnr+1)) / nb_jobs;
    const unsigned char *srcp = src->data[0] + slice_start * src_pitch;
    unsigned char *dstp = dst->data[0] + slice_start * dst_pitch;
    const int c2 = td->c2;
    const int c3 = td->c3;
    const int c4 = td->c4;
    const int c5 = td->c5;
    const int c6 = td->c6;
    const int c7 = td->c7;
    int x, y;
    for (y = slice_start; y < slice_end; y++) {
        for (x = 0; x < width; x += 4) {
            const int u = srcp[x + 0] - 128;
            const int v = srcp[x + 2] - 128;
            const int uvval = c2 * u + c3 * v + 1081344;
            dstp[x + 0] = CB((c4 * u + c5 * v + 8421376) >> 16);
            dstp[x + 1] = CB((65536 * (srcp[x + 1] - 16) + uvval) >> 16);
            dstp[x + 2] = CB((c6 * u + c7 * v + 8421376) >> 16);
            dstp[x + 3] = CB((65536 * (srcp[x + 3] - 16) + uvval) >> 16);
        }
        srcp += src_pitch;
        dstp += dst_pitch;
    }
    return 0;
}
