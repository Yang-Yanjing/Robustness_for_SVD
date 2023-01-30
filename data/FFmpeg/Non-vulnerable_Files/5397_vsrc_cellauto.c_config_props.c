static int config_props(AVFilterLink *outlink)
{
    CellAutoContext *cellauto = outlink->src->priv;
    outlink->w = cellauto->w;
    outlink->h = cellauto->h;
    outlink->time_base = av_inv_q(cellauto->frame_rate);
    return 0;
}
