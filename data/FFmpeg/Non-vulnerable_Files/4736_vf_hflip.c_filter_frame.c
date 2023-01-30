static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx  = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    ThreadData td;
    AVFrame *out;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    
    if (av_pix_fmt_desc_get(inlink->format)->flags & AV_PIX_FMT_FLAG_PAL)
        memcpy(out->data[1], in->data[1], AVPALETTE_SIZE);
    td.in = in, td.out = out;
    ctx->internal->execute(ctx, filter_slice, &td, NULL, FFMIN(outlink->h, ctx->graph->nb_threads));
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
