static int config_props_out(AVFilterLink *link)
{
    AVFilterContext *ctx = link->src;
    return config_props(ctx, link, 1);
}
