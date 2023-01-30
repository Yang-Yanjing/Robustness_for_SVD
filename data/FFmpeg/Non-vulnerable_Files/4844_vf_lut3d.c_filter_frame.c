static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterLink *outlink = inlink->dst->outputs[0];
    AVFrame *out = apply_lut(inlink, in);
    if (!out)
        return AVERROR(ENOMEM);
    return ff_filter_frame(outlink, out);
}
