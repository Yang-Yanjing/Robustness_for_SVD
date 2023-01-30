static int pulse_stream_wait(PulseData *s)
{
    pa_stream_state_t state;
    while ((state = pa_stream_get_state(s->stream)) != PA_STREAM_READY) {
        if (state == PA_STREAM_FAILED || state == PA_STREAM_TERMINATED)
            return AVERROR_EXTERNAL;
        pa_threaded_mainloop_wait(s->mainloop);
    }
    return 0;
}
