static void pulse_stream_state(pa_stream *stream, void *userdata)
{
    PulseData *s = userdata;
    if (stream != s->stream)
        return;
    switch (pa_stream_get_state(s->stream)) {
        case PA_STREAM_READY:
        case PA_STREAM_FAILED:
        case PA_STREAM_TERMINATED:
            pa_threaded_mainloop_signal(s->mainloop, 0);
        default:
            break;
    }
}
