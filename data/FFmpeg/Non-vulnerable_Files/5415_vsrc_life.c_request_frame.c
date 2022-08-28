static int request_frame(AVFilterLink *outlink)
{
    LifeContext *life = outlink->src->priv;
    AVFrame *picref = ff_get_video_buffer(outlink, life->w, life->h);
    if (!picref)
        return AVERROR(ENOMEM);
    picref->sample_aspect_ratio = (AVRational) {1, 1};
    picref->pts = life->pts++;
    life->draw(outlink->src, picref);
    evolve(outlink->src);
#ifdef DEBUG
    show_life_grid(outlink->src);
#endif
    return ff_filter_frame(outlink, picref);
}
