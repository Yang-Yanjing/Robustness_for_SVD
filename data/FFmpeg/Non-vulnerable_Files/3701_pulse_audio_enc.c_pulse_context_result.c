static void pulse_context_result(pa_context *ctx, int success, void *userdata)
{
    PulseData *s = userdata;
    if (s->ctx != ctx)
        return;
    s->last_result = success ? 0 : AVERROR_EXTERNAL;
    pa_threaded_mainloop_signal(s->mainloop, 0);
}
