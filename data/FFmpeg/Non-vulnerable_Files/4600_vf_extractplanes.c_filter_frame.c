static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    ExtractPlanesContext *s = ctx->priv;
    int i, eof = 0, ret = 0;
    for (i = 0; i < ctx->nb_outputs; i++) {
        AVFilterLink *outlink = ctx->outputs[i];
        const int idx = s->map[i];
        AVFrame *out;
        if (outlink->closed)
            continue;
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            ret = AVERROR(ENOMEM);
            break;
        }
        av_frame_copy_props(out, frame);
        if (s->is_packed) {
            extract_from_packed(out->data[0], out->linesize[0],
                                frame->data[0], frame->linesize[0],
                                outlink->w, outlink->h,
                                s->depth,
                                s->step, idx);
        } else {
            av_image_copy_plane(out->data[0], out->linesize[0],
                                frame->data[idx], frame->linesize[idx],
                                s->linesize[idx], outlink->h);
        }
        ret = ff_filter_frame(outlink, out);
        if (ret == AVERROR_EOF)
            eof++;
        else if (ret < 0)
            break;
    }
    av_frame_free(&frame);
    if (eof == ctx->nb_outputs)
        ret = AVERROR_EOF;
    else if (ret == AVERROR_EOF)
        ret = 0;
    return ret;
}
