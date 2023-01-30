int ff_pulse_audio_connect_context(pa_mainloop **pa_ml, pa_context **pa_ctx,
                                   const char *server, const char *description)
{
    int ret;
    pa_mainloop_api *pa_mlapi = NULL;
    enum PulseAudioContextState context_state = PULSE_CONTEXT_INITIALIZING;
    av_assert0(pa_ml);
    av_assert0(pa_ctx);
    *pa_ml = NULL;
    *pa_ctx = NULL;
    if (!(*pa_ml = pa_mainloop_new()))
        return AVERROR(ENOMEM);
    if (!(pa_mlapi = pa_mainloop_get_api(*pa_ml))) {
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    if (!(*pa_ctx = pa_context_new(pa_mlapi, description))) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    pa_context_set_state_callback(*pa_ctx, pa_state_cb, &context_state);
    if (pa_context_connect(*pa_ctx, server, 0, NULL) < 0) {
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    while (context_state == PULSE_CONTEXT_INITIALIZING)
        pa_mainloop_iterate(*pa_ml, 1, NULL);
    if (context_state == PULSE_CONTEXT_FINISHED) {
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    return 0;
  fail:
    ff_pulse_audio_disconnect_context(pa_ml, pa_ctx);
    return ret;
}
