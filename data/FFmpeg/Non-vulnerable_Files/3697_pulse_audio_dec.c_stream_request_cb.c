static void stream_request_cb(pa_stream *s, size_t length, void *userdata) {
    PulseData *p = userdata;
    pa_threaded_mainloop_signal(p->mainloop, 0);
}
