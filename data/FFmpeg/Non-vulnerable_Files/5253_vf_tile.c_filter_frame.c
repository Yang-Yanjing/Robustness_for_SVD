static int filter_frame(AVFilterLink *inlink, AVFrame *picref)
{
    AVFilterContext *ctx  = inlink->dst;
    TileContext *tile     = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    unsigned x0, y0;
    if (!tile->current) {
        tile->out_ref = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!tile->out_ref) {
            av_frame_free(&picref);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(tile->out_ref, picref);
        tile->out_ref->width  = outlink->w;
        tile->out_ref->height = outlink->h;
        
        if (tile->margin || tile->padding)
            ff_fill_rectangle(&tile->draw, &tile->blank,
                              tile->out_ref->data,
                              tile->out_ref->linesize,
                              0, 0, outlink->w, outlink->h);
    }
    get_current_tile_pos(ctx, &x0, &y0);
    ff_copy_rectangle2(&tile->draw,
                       tile->out_ref->data, tile->out_ref->linesize,
                       picref->data, picref->linesize,
                       x0, y0, 0, 0, inlink->w, inlink->h);
    av_frame_free(&picref);
    if (++tile->current == tile->nb_frames)
        return end_last_frame(ctx);
    return 0;
}
