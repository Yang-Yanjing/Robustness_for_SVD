static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    FOCContext *foc = ctx->priv;
    if (foc->xmax <= 0)
        foc->xmax = inlink->w - foc->obj_frame->width;
    if (foc->ymax <= 0)
        foc->ymax = inlink->h - foc->obj_frame->height;
    return 0;
}
