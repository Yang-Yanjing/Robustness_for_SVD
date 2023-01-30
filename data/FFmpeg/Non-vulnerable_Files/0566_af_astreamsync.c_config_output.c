static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    int id = outlink == ctx->outputs[1];
    outlink->sample_rate = ctx->inputs[id]->sample_rate;
    outlink->time_base   = ctx->inputs[id]->time_base;
    return 0;
}
