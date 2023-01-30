static void pulse_stream_result(pa_stream *stream, int success, void *userdata)
{
    PulseData *s = userdata;
    if (stream != s->stream)
        return;
    s->last_result = success ? 0 : AVERROR_EXTERNAL;
    pa_threaded_mainloop_signal(s->mainloop, 0);
}
