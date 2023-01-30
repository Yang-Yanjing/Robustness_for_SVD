static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    StackContext *s = inlink->dst->priv;
    return ff_framesync_filter_frame(&s->fs, inlink, in);
}
