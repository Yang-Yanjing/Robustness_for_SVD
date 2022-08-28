static int end_last_frame(AVFilterContext *ctx)
{
    TileContext *tile     = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out_buf = tile->out_ref;
    int ret;
    while (tile->current < tile->nb_frames)
        draw_blank_frame(ctx, out_buf);
    ret = ff_filter_frame(outlink, out_buf);
    tile->current = 0;
    return ret;
}
