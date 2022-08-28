static int config_props(AVFilterLink *outlink)
{
    LifeContext *life = outlink->src->priv;
    outlink->w = life->w;
    outlink->h = life->h;
    outlink->time_base = av_inv_q(life->frame_rate);
    return 0;
}
