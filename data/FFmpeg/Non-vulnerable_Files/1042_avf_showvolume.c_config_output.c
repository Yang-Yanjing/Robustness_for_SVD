static int config_output(AVFilterLink *outlink)
{
    ShowVolumeContext *s = outlink->src->priv;
    AVFilterLink *inlink = outlink->src->inputs[0];
    outlink->w = s->w;
    outlink->h = s->h * inlink->channels + (inlink->channels - 1) * s->b;
    outlink->sample_aspect_ratio = (AVRational){1,1};
    outlink->frame_rate = s->frame_rate;
    return 0;
}
