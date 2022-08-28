static int filter_frame(AVFilterLink *inlink, AVFrame *in) {
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = inlink->dst->outputs[0];
    RepeatFieldsContext *s = ctx->priv;
    AVFrame *out;
    int ret, i;
    int state = s->state;
    if (!s->frame) {
        s->frame = av_frame_clone(in);
        if (!s->frame)
            return AVERROR(ENOMEM);
        s->frame->pts = AV_NOPTS_VALUE;
    }
    out = s->frame;
    if ((state == 0 && !in->top_field_first) ||
        (state == 1 &&  in->top_field_first)) {
        av_log(ctx, AV_LOG_WARNING, "Unexpected field flags: "
                                    "state=%d top_field_first=%d repeat_first_field=%d\n",
                                    state, in->top_field_first, in->repeat_pict);
        state ^= 1;
    }
    if (state == 0) {
        AVFrame *new;
        new = av_frame_clone(in);
        if (!new)
            return AVERROR(ENOMEM);
        ret = ff_filter_frame(outlink, new);
        if (in->repeat_pict) {
            av_frame_make_writable(out);
            update_pts(outlink, out, in->pts, 2);
            for (i = 0; i < s->nb_planes; i++) {
                av_image_copy_plane(out->data[i], out->linesize[i] * 2,
                                    in->data[i], in->linesize[i] * 2,
                                    s->linesize[i], s->planeheight[i] / 2);
            }
            state = 1;
        }
    } else {
        for (i = 0; i < s->nb_planes; i++) {
            av_frame_make_writable(out);
            av_image_copy_plane(out->data[i] + out->linesize[i], out->linesize[i] * 2,
                                in->data[i] + in->linesize[i], in->linesize[i] * 2,
                                s->linesize[i], s->planeheight[i] / 2);
        }
        ret = ff_filter_frame(outlink, av_frame_clone(out));
        if (in->repeat_pict) {
            AVFrame *new;
            new = av_frame_clone(in);
            if (!new)
                return AVERROR(ENOMEM);
            ret = ff_filter_frame(outlink, new);
            state = 0;
        } else {
            av_frame_make_writable(out);
            update_pts(outlink, out, in->pts, 1);
            for (i = 0; i < s->nb_planes; i++) {
                av_image_copy_plane(out->data[i], out->linesize[i] * 2,
                                    in->data[i], in->linesize[i] * 2,
                                    s->linesize[i], s->planeheight[i] / 2);
            }
        }
    }
    s->state = state;
    av_frame_free(&in);
    return ret;
}
