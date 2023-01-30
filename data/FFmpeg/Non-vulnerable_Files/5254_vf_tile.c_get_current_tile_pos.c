static void get_current_tile_pos(AVFilterContext *ctx, unsigned *x, unsigned *y)
{
    TileContext *tile    = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    const unsigned tx = tile->current % tile->w;
    const unsigned ty = tile->current / tile->w;
    *x = tile->margin + (inlink->w + tile->padding) * tx;
    *y = tile->margin + (inlink->h + tile->padding) * ty;
}
