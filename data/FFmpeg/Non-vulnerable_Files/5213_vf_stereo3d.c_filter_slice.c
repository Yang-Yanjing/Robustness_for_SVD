static int filter_slice(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    Stereo3DContext *s = ctx->priv;
    ThreadData *td = arg;
    AVFrame *ileft = td->ileft;
    AVFrame *iright = td->iright;
    AVFrame *out = td->out;
    int height = s->out.height;
    int start = (height *  jobnr   ) / nb_jobs;
    int end   = (height * (jobnr+1)) / nb_jobs;
    uint8_t *dst = out->data[0];
    const int **ana_matrix = s->ana_matrix;
    int x, y, il, ir, o;
    const uint8_t *lsrc = ileft->data[0];
    const uint8_t *rsrc = iright->data[0];
    int out_width = s->out.width;
    for (y = start; y < end; y++) {
        o   = out->linesize[0] * y;
        il  = s->in_off_left[0]  + y * ileft->linesize[0];
        ir  = s->in_off_right[0] + y * iright->linesize[0];
        for (x = 0; x < out_width; x++, il += 3, ir += 3, o+= 3) {
            dst[o    ] = ana_convert(ana_matrix[0], lsrc + il, rsrc + ir);
            dst[o + 1] = ana_convert(ana_matrix[1], lsrc + il, rsrc + ir);
            dst[o + 2] = ana_convert(ana_matrix[2], lsrc + il, rsrc + ir);
        }
    }
    return 0;
}
