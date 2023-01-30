static av_cold int pulse_write_trailer(AVFormatContext *h)
{
    PulseData *s = h->priv_data;
    if (s->mainloop) {
        pa_threaded_mainloop_lock(s->mainloop);
        if (s->stream) {
            pa_stream_disconnect(s->stream);
            pa_stream_set_state_callback(s->stream, NULL, NULL);
            pa_stream_set_write_callback(s->stream, NULL, NULL);
            pa_stream_set_overflow_callback(s->stream, NULL, NULL);
            pa_stream_set_underflow_callback(s->stream, NULL, NULL);
            pa_stream_unref(s->stream);
            s->stream = NULL;
        }
        if (s->ctx) {
            pa_context_disconnect(s->ctx);
            pa_context_set_state_callback(s->ctx, NULL, NULL);
            pa_context_set_subscribe_callback(s->ctx, NULL, NULL);
            pa_context_unref(s->ctx);
            s->ctx = NULL;
        }
        pa_threaded_mainloop_unlock(s->mainloop);
        pa_threaded_mainloop_stop(s->mainloop);
        pa_threaded_mainloop_free(s->mainloop);
        s->mainloop = NULL;
    }
    return 0;
}
