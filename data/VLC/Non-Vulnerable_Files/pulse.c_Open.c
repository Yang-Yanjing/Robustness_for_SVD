                            uint32_t, void *);
static int Open (vlc_object_t *obj)
{
    services_discovery_t *sd = (services_discovery_t *)obj;
    pa_operation *op;
    pa_context *ctx;
    services_discovery_sys_t *sys = malloc (sizeof (*sys));
    if (unlikely(sys == NULL))
        return VLC_ENOMEM;
    ctx = vlc_pa_connect (obj, &sys->mainloop);
    if (ctx == NULL)
    {
        free (sys);
        return VLC_EGENERIC;
    }
    sd->p_sys = sys;
    sys->context = ctx;
    sys->root = NULL;
    /* Subscribe for source events */
    const pa_subscription_mask_t mask = PA_SUBSCRIPTION_MASK_SOURCE;
    pa_threaded_mainloop_lock (sys->mainloop);
    pa_context_set_subscribe_callback (ctx, ContextCallback, sd);
    op = pa_context_subscribe (ctx, mask, NULL, NULL);
    if (likely(op != NULL))
        pa_operation_unref (op);
    /* Enumerate existing sources */
    op = pa_context_get_source_info_list (ctx, SourceCallback, sd);
    if (likely(op != NULL))
    {
        //while (pa_operation_get_state (op) == PA_OPERATION_RUNNING)
        //    pa_threaded_mainloop_wait (sys->mainloop);
        pa_operation_unref (op);
    }
    pa_threaded_mainloop_unlock (sys->mainloop);
    return VLC_SUCCESS;
/*
error:
    pa_threaded_mainloop_unlock (sys->mainloop);
    vlc_pa_disconnect (obj, ctx, sys->mainloop);
    free (sys);
    return VLC_EGENERIC;*/
}
