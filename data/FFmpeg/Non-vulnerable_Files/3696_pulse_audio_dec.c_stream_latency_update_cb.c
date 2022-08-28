static void stream_latency_update_cb(pa_stream *s, void *userdata) {
    PulseData *p = userdata;
    pa_threaded_mainloop_signal(p->mainloop, 0);
}
