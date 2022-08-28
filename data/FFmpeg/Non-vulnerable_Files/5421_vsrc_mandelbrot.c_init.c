AVFILTER_DEFINE_CLASS(mandelbrot);
static av_cold int init(AVFilterContext *ctx)
{
    MBContext *mb = ctx->priv;
    mb->bailout *= mb->bailout;
    mb->start_scale /=mb->h;
    mb->end_scale /=mb->h;
    mb->cache_allocated = mb->w * mb->h * 3;
    mb->cache_used = 0;
    mb->point_cache= av_malloc_array(mb->cache_allocated, sizeof(*mb->point_cache));
    mb-> next_cache= av_malloc_array(mb->cache_allocated, sizeof(*mb-> next_cache));
    mb-> zyklus    = av_malloc_array(mb->maxiter + 16, sizeof(*mb->zyklus));
    return 0;
}
