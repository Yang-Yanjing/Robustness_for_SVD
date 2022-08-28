static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    W3FDIFContext *s = ctx->priv;
    do {
        int ret;
        if (s->eof)
            return AVERROR_EOF;
        ret = ff_request_frame(ctx->inputs[0]);
        if (ret == AVERROR_EOF && s->cur) {
            AVFrame *next = av_frame_clone(s->next);
            if (!next)
                return AVERROR(ENOMEM);
            next->pts = s->next->pts * 2 - s->cur->pts;
            filter_frame(ctx->inputs[0], next);
            s->eof = 1;
        } else if (ret < 0) {
            return ret;
        }
    } while (!s->cur);
    return 0;
}
