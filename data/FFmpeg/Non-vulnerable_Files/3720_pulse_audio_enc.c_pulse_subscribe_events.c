static int pulse_subscribe_events(PulseData *s)
{
    pa_operation *op;
    pa_threaded_mainloop_lock(s->mainloop);
    op = pa_context_subscribe(s->ctx, PA_SUBSCRIPTION_MASK_SINK_INPUT, pulse_context_result, s);
    return pulse_finish_context_operation(s, op, "pa_context_subscribe");
}
