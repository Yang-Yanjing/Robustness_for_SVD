static int config_output(AVFilterLink *outlink)
{
    AVFilterLink *inlink = outlink->src->inputs[0];
    outlink->time_base.num = inlink->time_base.num;
    outlink->time_base.den = inlink->time_base.den * 2;
    outlink->frame_rate.num = inlink->frame_rate.num * 2;
    outlink->frame_rate.den = inlink->frame_rate.den;
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    return 0;
}
