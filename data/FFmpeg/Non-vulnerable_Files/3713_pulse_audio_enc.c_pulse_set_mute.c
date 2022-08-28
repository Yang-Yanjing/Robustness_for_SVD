static int pulse_set_mute(PulseData *s)
{
    pa_operation *op;
    pa_threaded_mainloop_lock(s->mainloop);
    op = pa_context_set_sink_input_mute(s->ctx, pa_stream_get_index(s->stream),
                                        s->mute, pulse_context_result, s);
    return pulse_finish_context_operation(s, op, "pa_context_set_sink_input_mute");
}
