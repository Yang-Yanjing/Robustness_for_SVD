static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    AVFilterContext *ctx = inlink->dst;
    int i, ret = AVERROR_EOF;
    for (i = 0; i < ctx->nb_outputs; i++) {
        AVFrame *buf_out;
        if (ctx->outputs[i]->closed)
            continue;
        buf_out = av_frame_clone(frame);
        if (!buf_out) {
            ret = AVERROR(ENOMEM);
            break;
        }
        ret = ff_filter_frame(ctx->outputs[i], buf_out);
        if (ret < 0)
            break;
    }
    av_frame_free(&frame);
    return ret;
}
