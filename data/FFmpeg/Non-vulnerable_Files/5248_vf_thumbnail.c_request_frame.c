static int request_frame(AVFilterLink *link)
{
    AVFilterContext *ctx = link->src;
    ThumbContext *thumb = ctx->priv;
    

    do {
        int ret = ff_request_frame(ctx->inputs[0]);
        if (ret == AVERROR_EOF && thumb->n) {
            ret = ff_filter_frame(link, get_best_frame(ctx));
            if (ret < 0)
                return ret;
            ret = AVERROR_EOF;
        }
        if (ret < 0)
            return ret;
    } while (thumb->n);
    return 0;
}
