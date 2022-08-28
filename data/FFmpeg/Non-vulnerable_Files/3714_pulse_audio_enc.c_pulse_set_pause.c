static int pulse_set_pause(PulseData *s, int pause)
{
    pa_operation *op;
    pa_threaded_mainloop_lock(s->mainloop);
    op = pa_stream_cork(s->stream, pause, pulse_stream_result, s);
    return pulse_finish_stream_operation(s, op, "pa_stream_cork");
}
