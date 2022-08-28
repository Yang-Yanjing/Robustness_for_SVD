static int request_frame(AVFilterLink *link)
{
    AVFilterContext *ctx = link->src;
    YADIFContext *yadif = ctx->priv;
    if (yadif->frame_pending) {
        return_frame(ctx, 1);
        return 0;
    }
    do {
        int ret;
        if (yadif->eof)
            return AVERROR_EOF;
        ret  = ff_request_frame(link->src->inputs[0]);
        if (ret == AVERROR_EOF && yadif->cur) {
            AVFrame *next = av_frame_clone(yadif->next);
            if (!next)
                return AVERROR(ENOMEM);
            next->pts = yadif->next->pts * 2 - yadif->cur->pts;
            filter_frame(link->src->inputs[0], next);
            yadif->eof = 1;
        } else if (ret < 0) {
            return ret;
        }
    } while (!yadif->prev);
    return 0;
}
