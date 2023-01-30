static int config_props_ref(AVFilterLink *outlink)
{
    AVFilterLink *inlink = outlink->src->inputs[1];
    outlink->w = inlink->w;
    outlink->h = inlink->h;
    outlink->sample_aspect_ratio = inlink->sample_aspect_ratio;
    outlink->time_base = inlink->time_base;
    return 0;
}
