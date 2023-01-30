static void find_next_delta_ts(AVFilterContext *ctx, int64_t *seg_delta)
{
    ConcatContext *cat = ctx->priv;
    unsigned i = cat->cur_idx;
    unsigned imax = i + ctx->nb_outputs;
    int64_t pts;
    pts = cat->in[i++].pts;
    for (; i < imax; i++)
        pts = FFMAX(pts, cat->in[i].pts);
    cat->delta_ts += pts;
    *seg_delta = pts;
}
