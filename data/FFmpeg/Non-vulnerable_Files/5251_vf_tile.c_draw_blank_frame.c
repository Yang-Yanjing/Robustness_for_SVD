static void draw_blank_frame(AVFilterContext *ctx, AVFrame *out_buf)
{
    TileContext *tile    = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    unsigned x0, y0;
    get_current_tile_pos(ctx, &x0, &y0);
    ff_fill_rectangle(&tile->draw, &tile->blank,
                      out_buf->data, out_buf->linesize,
                      x0, y0, inlink->w, inlink->h);
    tile->current++;
}
