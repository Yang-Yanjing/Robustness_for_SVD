static void context_state_cb(pa_context *c, void *userdata) {
    PulseData *p = userdata;
    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_READY:
        case PA_CONTEXT_TERMINATED:
        case PA_CONTEXT_FAILED:
            pa_threaded_mainloop_signal(p->mainloop, 0);
            break;
    }
}
