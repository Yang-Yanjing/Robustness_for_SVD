static void pulse_event(pa_context *ctx, pa_subscription_event_type_t t,
                        uint32_t idx, void *userdata)
{
    AVFormatContext *h = userdata;
    PulseData *s = h->priv_data;
    if (s->ctx != ctx)
        return;
    if ((t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == PA_SUBSCRIPTION_EVENT_SINK_INPUT) {
        if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_CHANGE)
            
            pulse_update_sink_input_info(h);
    }
}
