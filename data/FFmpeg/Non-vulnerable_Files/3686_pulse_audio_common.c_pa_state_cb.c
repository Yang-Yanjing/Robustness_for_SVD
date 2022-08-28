static void pa_state_cb(pa_context *c, void *userdata)
{
    enum PulseAudioContextState *context_state = userdata;
    switch  (pa_context_get_state(c)) {
    case PA_CONTEXT_FAILED:
    case PA_CONTEXT_TERMINATED:
        *context_state = PULSE_CONTEXT_FINISHED;
        break;
    case PA_CONTEXT_READY:
        *context_state = PULSE_CONTEXT_READY;
        break;
    default:
        break;
    }
}
