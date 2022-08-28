static int filter_frame(AVFilterLink *link, AVFrame *frame)
{
    AVFilterContext *ctx = link->dst;
    YADIFContext *yadif = ctx->priv;
    av_assert0(frame);
    if (yadif->frame_pending)
        return_frame(ctx, 1);
    if (yadif->prev)
        av_frame_free(&yadif->prev);
    yadif->prev = yadif->cur;
    yadif->cur  = yadif->next;
    yadif->next = frame;
    if (!yadif->cur &&
        !(yadif->cur = av_frame_clone(yadif->next)))
        return AVERROR(ENOMEM);
    if (checkstride(yadif, yadif->next, yadif->cur)) {
        av_log(ctx, AV_LOG_VERBOSE, "Reallocating frame due to differing stride\n");
        fixstride(link, yadif->next);
    }
    if (checkstride(yadif, yadif->next, yadif->cur))
        fixstride(link, yadif->cur);
    if (yadif->prev && checkstride(yadif, yadif->next, yadif->prev))
        fixstride(link, yadif->prev);
    if (checkstride(yadif, yadif->next, yadif->cur) || (yadif->prev && checkstride(yadif, yadif->next, yadif->prev))) {
        av_log(ctx, AV_LOG_ERROR, "Failed to reallocate frame\n");
        return -1;
    }
    if (!yadif->prev)
        return 0;
    if ((yadif->deint && !yadif->cur->interlaced_frame) ||
        ctx->is_disabled ||
        (yadif->deint && !yadif->prev->interlaced_frame && yadif->prev->repeat_pict) ||
        (yadif->deint && !yadif->next->interlaced_frame && yadif->next->repeat_pict)
    ) {
        yadif->out  = av_frame_clone(yadif->cur);
        if (!yadif->out)
            return AVERROR(ENOMEM);
        av_frame_free(&yadif->prev);
        if (yadif->out->pts != AV_NOPTS_VALUE)
            yadif->out->pts *= 2;
        return ff_filter_frame(ctx->outputs[0], yadif->out);
    }
    yadif->out = ff_get_video_buffer(ctx->outputs[0], link->w, link->h);
    if (!yadif->out)
        return AVERROR(ENOMEM);
    av_frame_copy_props(yadif->out, yadif->cur);
    yadif->out->interlaced_frame = 0;
    if (yadif->out->pts != AV_NOPTS_VALUE)
        yadif->out->pts *= 2;
    return return_frame(ctx, 0);
}
