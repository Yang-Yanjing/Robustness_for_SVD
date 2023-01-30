static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    AVFilterContext *ctx = inlink->dst;
    AlphaMergeContext *merge = ctx->priv;
    int ret = 0;
    int is_alpha = (inlink == ctx->inputs[1]);
    struct FFBufQueue *queue =
        (is_alpha ? &merge->queue_alpha : &merge->queue_main);
    ff_bufqueue_add(ctx, queue, buf);
    do {
        AVFrame *main_buf, *alpha_buf;
        if (!ff_bufqueue_peek(&merge->queue_main, 0) ||
            !ff_bufqueue_peek(&merge->queue_alpha, 0)) break;
        main_buf = ff_bufqueue_get(&merge->queue_main);
        alpha_buf = ff_bufqueue_get(&merge->queue_alpha);
        merge->frame_requested = 0;
        draw_frame(ctx, main_buf, alpha_buf);
        ret = ff_filter_frame(ctx->outputs[0], main_buf);
        av_frame_free(&alpha_buf);
    } while (ret >= 0);
    return ret;
}
