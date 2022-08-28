static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    return connect_ports(ctx, inlink);
}
