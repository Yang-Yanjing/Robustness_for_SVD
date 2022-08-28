static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    RemoveGrainContext *s = ctx->priv;
    ThreadData td;
    AVFrame *out;
    int i;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    for (i = 0; i < s->nb_planes; i++) {
        uint8_t *dst = out->data[i];
        uint8_t *src = in->data[i];
        if (s->mode[i] == 0) {
            av_image_copy_plane(dst, out->linesize[i],
                                src, in->linesize[i],
                                s->planewidth[i], s->planeheight[i]);
            continue;
        }
        memcpy(dst, src, s->planewidth[i]);
        td.in = in; td.out = out; td.plane = i;
        ctx->internal->execute(ctx, filter_slice, &td, NULL,
                               FFMIN(s->planeheight[i], ctx->graph->nb_threads));
        src = in->data[i] + (s->planeheight[i] - 1) * in->linesize[i];
        dst = out->data[i] + (s->planeheight[i] - 1) * out->linesize[i];
        memcpy(dst, src, s->planewidth[i]);
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
