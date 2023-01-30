static int pulse_flash_stream(PulseData *s)
{
    pa_operation *op;
    pa_threaded_mainloop_lock(s->mainloop);
    op = pa_stream_flush(s->stream, pulse_stream_result, s);
    return pulse_finish_stream_operation(s, op, "pa_stream_flush");
}
