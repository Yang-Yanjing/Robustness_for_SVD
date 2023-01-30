static int request_frame(AVFilterLink *outlink)
{
    CellAutoContext *cellauto = outlink->src->priv;
    AVFrame *picref = ff_get_video_buffer(outlink, cellauto->w, cellauto->h);
    if (!picref)
        return AVERROR(ENOMEM);
    picref->sample_aspect_ratio = (AVRational) {1, 1};
    if (cellauto->generation == 0 && cellauto->start_full) {
        int i;
        for (i = 0; i < cellauto->h-1; i++)
            evolve(outlink->src);
    }
    fill_picture(outlink->src, picref);
    evolve(outlink->src);
    picref->pts = cellauto->pts++;
#ifdef DEBUG
    show_cellauto_row(outlink->src);
#endif
    return ff_filter_frame(outlink, picref);
}
