static int config_props(AVFilterLink *link)
{
    BufferSourceContext *c = link->src->priv;
    switch (link->type) {
    case AVMEDIA_TYPE_VIDEO:
        link->w = c->w;
        link->h = c->h;
        link->sample_aspect_ratio = c->pixel_aspect;
        break;
    case AVMEDIA_TYPE_AUDIO:
        if (!c->channel_layout)
            c->channel_layout = link->channel_layout;
        break;
    default:
        return AVERROR(EINVAL);
    }
    link->time_base = c->time_base;
    link->frame_rate = c->frame_rate;
    return 0;
}
