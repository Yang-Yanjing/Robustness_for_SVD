static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    ATADenoiseContext *s = ctx->priv;
    AVFrame *out, *in;
    int i;
    if (s->q.available != s->size) {
        if (s->q.available < s->mid) {
            out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
            if (!out)
                return AVERROR(ENOMEM);
            for (i = 0; i < s->mid; i++)
                ff_bufqueue_add(ctx, &s->q, av_frame_clone(out));
            av_frame_free(&out);
        }
        if (s->q.available < s->size) {
            ff_bufqueue_add(ctx, &s->q, buf);
            s->available++;
        }
        return 0;
    }
    in = ff_bufqueue_peek(&s->q, s->mid);
    if (!ctx->is_disabled) {
        ThreadData td;
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            av_frame_free(&buf);
            return AVERROR(ENOMEM);
        }
        for (i = 0; i < s->size; i++) {
            AVFrame *frame = ff_bufqueue_peek(&s->q, i);
            s->data[0][i] = frame->data[0];
            s->data[1][i] = frame->data[1];
            s->data[2][i] = frame->data[2];
            s->linesize[0][i] = frame->linesize[0];
            s->linesize[1][i] = frame->linesize[1];
            s->linesize[2][i] = frame->linesize[2];
        }
        td.in = in; td.out = out;
        ctx->internal->execute(ctx, s->filter_slice, &td, NULL,
                               FFMIN3(s->planeheight[1],
                                      s->planeheight[2],
                                      ctx->graph->nb_threads));
        av_frame_copy_props(out, in);
    } else {
        out = av_frame_clone(in);
        if (!out) {
            av_frame_free(&buf);
            return AVERROR(ENOMEM);
        }
    }
    in = ff_bufqueue_get(&s->q);
    av_frame_free(&in);
    ff_bufqueue_add(ctx, &s->q, buf);
    return ff_filter_frame(outlink, out);
}
