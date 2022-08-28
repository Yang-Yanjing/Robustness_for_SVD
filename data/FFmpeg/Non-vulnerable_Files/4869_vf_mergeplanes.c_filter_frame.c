AVFILTER_DEFINE_CLASS(mergeplanes);
static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    MergePlanesContext *s = inlink->dst->priv;
    return ff_framesync_filter_frame(&s->fs, inlink, in);
}
