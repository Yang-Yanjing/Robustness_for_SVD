static int config_props_in(AVFilterLink *link)
{
    AVFilterContext *ctx = link->dst;
    return config_props(ctx, link, 0);
}
