static int config_output(AVFilterLink *outlink)
{
    AVFilterLink *inlink = outlink->src->inputs[0];
    outlink->w = inlink->w*2;
    outlink->h = inlink->h*2;
    av_log(inlink->dst, AV_LOG_VERBOSE, "fmt:%s size:%dx%d -> size:%dx%d\n",
           av_get_pix_fmt_name(inlink->format),
           inlink->w, inlink->h, outlink->w, outlink->h);
    return 0;
}
