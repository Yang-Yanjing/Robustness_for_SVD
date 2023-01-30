static void handle_trimming(AVFilterContext *ctx)
{
    ASyncContext *s = ctx->priv;
    if (s->pts < s->first_pts) {
        int delta = FFMIN(s->first_pts - s->pts, avresample_available(s->avr));
        av_log(ctx, AV_LOG_VERBOSE, "Trimming %d samples from start\n",
               delta);
        avresample_read(s->avr, NULL, delta);
        s->pts += delta;
    } else if (s->first_frame)
        s->pts = s->first_pts;
}
