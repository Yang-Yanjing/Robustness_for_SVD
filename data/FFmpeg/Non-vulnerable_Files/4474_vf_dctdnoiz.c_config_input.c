DECLARE_COLOR_FUNCS(bgr, 2, 1, 0)
static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    DCTdnoizContext *s = ctx->priv;
    int i, x, y, bx, by, linesize, *iweights, max_slice_h, slice_h;
    const int bsize = 1 << s->n;
    switch (inlink->format) {
    case AV_PIX_FMT_BGR24:
        s->color_decorrelation = color_decorrelation_bgr;
        s->color_correlation   = color_correlation_bgr;
        break;
    case AV_PIX_FMT_RGB24:
        s->color_decorrelation = color_decorrelation_rgb;
        s->color_correlation   = color_correlation_rgb;
        break;
    default:
        av_assert0(0);
    }
    s->pr_width  = inlink->w - (inlink->w - bsize) % s->step;
    s->pr_height = inlink->h - (inlink->h - bsize) % s->step;
    if (s->pr_width != inlink->w)
        av_log(ctx, AV_LOG_WARNING, "The last %d horizontal pixels won't be denoised\n",
               inlink->w - s->pr_width);
    if (s->pr_height != inlink->h)
        av_log(ctx, AV_LOG_WARNING, "The last %d vertical pixels won't be denoised\n",
               inlink->h - s->pr_height);
    max_slice_h = s->pr_height / ((s->bsize - 1) * 2);
    s->nb_threads = FFMIN3(MAX_THREADS, ctx->graph->nb_threads, max_slice_h);
    av_log(ctx, AV_LOG_DEBUG, "threads: [max=%d hmax=%d user=%d] => %d\n",
           MAX_THREADS, max_slice_h, ctx->graph->nb_threads, s->nb_threads);
    s->p_linesize = linesize = FFALIGN(s->pr_width, 32);
    for (i = 0; i < 2; i++) {
        s->cbuf[i][0] = av_malloc_array(linesize * s->pr_height, sizeof(*s->cbuf[i][0]));
        s->cbuf[i][1] = av_malloc_array(linesize * s->pr_height, sizeof(*s->cbuf[i][1]));
        s->cbuf[i][2] = av_malloc_array(linesize * s->pr_height, sizeof(*s->cbuf[i][2]));
        if (!s->cbuf[i][0] || !s->cbuf[i][1] || !s->cbuf[i][2])
            return AVERROR(ENOMEM);
    }
    

    if (s->expr_str) {
        for (i = 0; i < s->nb_threads; i++) {
            int ret = av_expr_parse(&s->expr[i], s->expr_str, var_names,
                                    NULL, NULL, NULL, NULL, 0, ctx);
            if (ret < 0)
                return ret;
        }
    }
    


    slice_h = (int)ceilf(s->pr_height / (float)s->nb_threads) + (s->bsize - 1) * 2;
    for (i = 0; i < s->nb_threads; i++) {
        s->slices[i] = av_malloc_array(linesize, slice_h * sizeof(*s->slices[i]));
        if (!s->slices[i])
            return AVERROR(ENOMEM);
    }
    s->weights = av_malloc(s->pr_height * linesize * sizeof(*s->weights));
    if (!s->weights)
        return AVERROR(ENOMEM);
    iweights = av_calloc(s->pr_height, linesize * sizeof(*iweights));
    if (!iweights)
        return AVERROR(ENOMEM);
    for (y = 0; y < s->pr_height - bsize + 1; y += s->step)
        for (x = 0; x < s->pr_width - bsize + 1; x += s->step)
            for (by = 0; by < bsize; by++)
                for (bx = 0; bx < bsize; bx++)
                    iweights[(y + by)*linesize + x + bx]++;
    for (y = 0; y < s->pr_height; y++)
        for (x = 0; x < s->pr_width; x++)
            s->weights[y*linesize + x] = 1. / iweights[y*linesize + x];
    av_free(iweights);
    return 0;
}
