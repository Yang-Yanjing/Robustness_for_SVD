static av_cold int init(AVFilterContext *ctx)
{
    FadeContext *s = ctx->priv;
    s->fade_per_frame = (1 << 16) / s->nb_frames;
    s->fade_state = VF_FADE_WAITING;
    if (s->duration != 0) {
        
        s->nb_frames = 0; 
    }
    
    if (s->start_frame || s->nb_frames) {
        av_log(ctx, AV_LOG_VERBOSE,
               "type:%s start_frame:%d nb_frames:%d alpha:%d\n",
               s->type == FADE_IN ? "in" : "out", s->start_frame,
               s->nb_frames,s->alpha);
    }
    if (s->start_time || s->duration) {
        av_log(ctx, AV_LOG_VERBOSE,
               "type:%s start_time:%f duration:%f alpha:%d\n",
               s->type == FADE_IN ? "in" : "out", (s->start_time / (double)AV_TIME_BASE),
               (s->duration / (double)AV_TIME_BASE),s->alpha);
    }
    s->black_fade = !memcmp(s->color_rgba, "\x00\x00\x00\xff", 4);
    return 0;
}
