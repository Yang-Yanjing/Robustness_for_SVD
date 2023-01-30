void ff_pulse_audio_disconnect_context(pa_mainloop **pa_ml, pa_context **pa_ctx)
{
    av_assert0(pa_ml);
    av_assert0(pa_ctx);
    if (*pa_ctx) {
        pa_context_set_state_callback(*pa_ctx, NULL, NULL);
        pa_context_disconnect(*pa_ctx);
        pa_context_unref(*pa_ctx);
    }
    if (*pa_ml)
        pa_mainloop_free(*pa_ml);
    *pa_ml = NULL;
    *pa_ctx = NULL;
}
