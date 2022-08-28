static void stream_state_cb(pa_stream *s, void * userdata) {
    PulseData *p = userdata;
    switch (pa_stream_get_state(s)) {
        case PA_STREAM_READY:
        case PA_STREAM_FAILED:
        case PA_STREAM_TERMINATED:
            pa_threaded_mainloop_signal(p->mainloop, 0);
            break;
    }
}
