static int geq_filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    int plane;
    GEQContext *geq = inlink->dst->priv;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    AVFrame *out;
    double values[VAR_VARS_NB] = {
        [VAR_N] = inlink->frame_count,
        [VAR_T] = in->pts == AV_NOPTS_VALUE ? NAN : in->pts * av_q2d(inlink->time_base),
    };
    geq->picref = in;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    for (plane = 0; plane < geq->planes && out->data[plane]; plane++) {
        int x, y;
        uint8_t *dst = out->data[plane];
        const int linesize = out->linesize[plane];
        const int w = (plane == 1 || plane == 2) ? FF_CEIL_RSHIFT(inlink->w, geq->hsub) : inlink->w;
        const int h = (plane == 1 || plane == 2) ? FF_CEIL_RSHIFT(inlink->h, geq->vsub) : inlink->h;
        values[VAR_W]  = w;
        values[VAR_H]  = h;
        values[VAR_SW] = w / (double)inlink->w;
        values[VAR_SH] = h / (double)inlink->h;
        for (y = 0; y < h; y++) {
            values[VAR_Y] = y;
            for (x = 0; x < w; x++) {
                values[VAR_X] = x;
                dst[x] = av_expr_eval(geq->e[plane], values, geq);
            }
            dst += linesize;
        }
    }
    av_frame_free(&geq->picref);
    return ff_filter_frame(outlink, out);
}
