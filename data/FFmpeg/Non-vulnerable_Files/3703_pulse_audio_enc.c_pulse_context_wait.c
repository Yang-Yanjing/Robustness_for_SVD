static int pulse_context_wait(PulseData *s)
{
    pa_context_state_t state;
    while ((state = pa_context_get_state(s->ctx)) != PA_CONTEXT_READY) {
        if (state == PA_CONTEXT_FAILED || state == PA_CONTEXT_TERMINATED)
            return AVERROR_EXTERNAL;
        pa_threaded_mainloop_wait(s->mainloop);
    }
    return 0;
}
