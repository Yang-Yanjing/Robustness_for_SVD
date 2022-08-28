static int config_props(AVFilterLink *outlink)
{
    TestSourceContext *test = outlink->src->priv;
    outlink->w = test->w;
    outlink->h = test->h;
    outlink->sample_aspect_ratio = test->sar;
    outlink->frame_rate = test->frame_rate;
    outlink->time_base  = test->time_base;
    return 0;
}
