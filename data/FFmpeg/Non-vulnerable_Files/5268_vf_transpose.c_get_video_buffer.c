static AVFrame *get_video_buffer(AVFilterLink *inlink, int w, int h)
{
    TransContext *trans = inlink->dst->priv;
    return trans->passthrough ?
        ff_null_get_video_buffer   (inlink, w, h) :
        ff_default_get_video_buffer(inlink, w, h);
}
