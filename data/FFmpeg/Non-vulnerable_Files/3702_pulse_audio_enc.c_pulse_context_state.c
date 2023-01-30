static void pulse_context_state(pa_context *ctx, void *userdata)
{
    PulseData *s = userdata;
    if (s->ctx != ctx)
        return;
    switch (pa_context_get_state(ctx)) {
        case PA_CONTEXT_READY:
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
            pa_threaded_mainloop_signal(s->mainloop, 0);
        default:
            break;
    }
}
