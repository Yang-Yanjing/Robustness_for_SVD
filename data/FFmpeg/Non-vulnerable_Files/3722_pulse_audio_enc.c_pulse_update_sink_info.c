static int pulse_update_sink_info(AVFormatContext *h)
{
    PulseData *s = h->priv_data;
    pa_operation *op;
    if (!(op = pa_context_get_sink_info_by_name(s->ctx, s->device,
                                                pulse_audio_sink_device_cb, s))) {
        av_log(s, AV_LOG_ERROR, "pa_context_get_sink_info_by_name failed.\n");
        return AVERROR_EXTERNAL;
    }
    while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(s->mainloop);
    pa_operation_unref(op);
    return 0;
}
