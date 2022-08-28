static int pulse_finish_stream_operation(PulseData *s, pa_operation *op, const char *name)
{
    if (!op) {
        pa_threaded_mainloop_unlock(s->mainloop);
        av_log(s, AV_LOG_ERROR, "%s failed.\n", name);
        return AVERROR_EXTERNAL;
    }
    s->last_result = 2;
    while (s->last_result == 2)
        pa_threaded_mainloop_wait(s->mainloop);
    pa_operation_unref(op);
    pa_threaded_mainloop_unlock(s->mainloop);
    if (s->last_result != 0)
        av_log(s, AV_LOG_ERROR, "%s failed.\n", name);
    return s->last_result;
}
