static int filter_slice(AVFilterContext *ctx, void *arg, int jobnr, int nb_jobs)
{
    NoiseContext *s = ctx->priv;
    ThreadData *td = arg;
    int plane;
    for (plane = 0; plane < s->nb_planes; plane++) {
        const int height = s->height[plane];
        const int start  = (height *  jobnr   ) / nb_jobs;
        const int end    = (height * (jobnr+1)) / nb_jobs;
        noise(td->out->data[plane] + start * td->out->linesize[plane],
              td->in->data[plane]  + start * td->in->linesize[plane],
              td->out->linesize[plane], td->in->linesize[plane],
              s->bytewidth[plane], start, end, s, plane);
    }
    return 0;
}
