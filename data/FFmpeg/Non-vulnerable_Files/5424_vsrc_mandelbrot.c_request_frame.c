static int request_frame(AVFilterLink *link)
{
    MBContext *mb = link->src->priv;
    AVFrame *picref = ff_get_video_buffer(link, mb->w, mb->h);
    if (!picref)
        return AVERROR(ENOMEM);
    picref->sample_aspect_ratio = (AVRational) {1, 1};
    picref->pts = mb->pts++;
    draw_mandelbrot(link->src, (uint32_t*)picref->data[0], picref->linesize[0]/4, picref->pts);
    return ff_filter_frame(link, picref);
}
