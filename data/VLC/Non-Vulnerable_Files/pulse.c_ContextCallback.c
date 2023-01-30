}
static void ContextCallback(pa_context *ctx, pa_subscription_event_type_t type,
                            uint32_t idx, void *userdata)
{
    services_discovery_t *sd = userdata;
    pa_operation *op;
    assert ((type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK)
                                              == PA_SUBSCRIPTION_EVENT_SOURCE);
    switch (type & PA_SUBSCRIPTION_EVENT_TYPE_MASK)
    {
      case PA_SUBSCRIPTION_EVENT_NEW:
      case PA_SUBSCRIPTION_EVENT_CHANGE:
        op = pa_context_get_source_info_by_index(ctx, idx, SourceCallback, sd);
        if (likely(op != NULL))
            pa_operation_unref(op);
        break;
      case PA_SUBSCRIPTION_EVENT_REMOVE:
        RemoveSource (sd, idx);
        break;
    }
}
