static int filter_slice(AVFilterContext *ctx, void *arg, int job, int nb_jobs)
{
    ThreadData *td = (ThreadData*)arg;
    AVFrame *in = td->in;
    AVFrame *out = td->out;
    const int w = td->w, h = td->h;
    const int xcenter = td->xcenter;
    const int ycenter = td->ycenter;
    const int start = (h *  job   ) / nb_jobs;
    const int end   = (h * (job+1)) / nb_jobs;
    const int plane = td->plane;
    const int inlinesize = in->linesize[plane];
    const int outlinesize = out->linesize[plane];
    const uint8_t *indata = in->data[plane];
    uint8_t *outrow = out->data[plane] + start * outlinesize;
    int i;
    for (i = start; i < end; i++, outrow += outlinesize) {
        const int off_y = i - ycenter;
        uint8_t *out = outrow;
        int j;
        for (j = 0; j < w; j++) {
            const int off_x = j - xcenter;
            const int64_t radius_mult = td->correction[j + i*w];
            const int x = xcenter + ((radius_mult * off_x + (1<<23))>>24);
            const int y = ycenter + ((radius_mult * off_y + (1<<23))>>24);
            const char isvalid = x > 0 && x < w - 1 && y > 0 && y < h - 1;
            *out++ =  isvalid ? indata[y * inlinesize + x] : 0;
        }
    }
    return 0;
}
