static int filter_frame(AVFilterLink *inlink, AVFrame *inpicref)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    NoiseContext *n = ctx->priv;
    ThreadData td;
    AVFrame *out;
    int comp, i;
    if (av_frame_is_writable(inpicref)) {
        out = inpicref;
    } else {
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            av_frame_free(&inpicref);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, inpicref);
    }
    for (comp = 0; comp < 4; comp++) {
        FilterParams *fp = &n->param[comp];
        if ((!fp->rand_shift_init || (fp->flags & NOISE_TEMPORAL)) && fp->strength) {
            for (i = 0; i < MAX_RES; i++) {
                fp->rand_shift[i] = av_lfg_get(&fp->lfg) & (MAX_SHIFT - 1);
            }
            fp->rand_shift_init = 1;
        }
    }
    td.in = inpicref; td.out = out;
    ctx->internal->execute(ctx, filter_slice, &td, NULL, FFMIN(n->height[0], ctx->graph->nb_threads));
    emms_c();
    if (inpicref != out)
        av_frame_free(&inpicref);
    return ff_filter_frame(outlink, out);
}
