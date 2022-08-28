int av_timecode_init(AVTimecode *tc, AVRational rate, int flags, int frame_start, void *log_ctx)
{
    memset(tc, 0, sizeof(*tc));
    tc->start = frame_start;
    tc->flags = flags;
    tc->rate  = rate;
    tc->fps   = fps_from_frame_rate(rate);
    return check_timecode(log_ctx, tc);
}
