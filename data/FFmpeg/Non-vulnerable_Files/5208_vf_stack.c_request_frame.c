static int request_frame(AVFilterLink *outlink)
{
    StackContext *s = outlink->src->priv;
    return ff_framesync_request_frame(&s->fs, outlink);
}
