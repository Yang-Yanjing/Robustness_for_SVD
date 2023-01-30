static int pulse_update_sink_input_info(AVFormatContext *h)
{
    PulseData *s = h->priv_data;
    pa_operation *op;
    enum pa_operation_state op_state;
    pa_mainloop *ml = NULL;
    pa_context *ctx = NULL;
    int ret = 0;
    if ((ret = ff_pulse_audio_connect_context(&ml, &ctx, s->server, "Update sink input information")) < 0)
        return ret;
    if (!(op = pa_context_get_sink_input_info(ctx, pa_stream_get_index(s->stream),
                                              pulse_audio_sink_input_cb, h))) {
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    while ((op_state = pa_operation_get_state(op)) == PA_OPERATION_RUNNING)
        pa_mainloop_iterate(ml, 1, NULL);
    pa_operation_unref(op);
    if (op_state != PA_OPERATION_DONE) {
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
  fail:
    ff_pulse_audio_disconnect_context(&ml, &ctx);
    if (ret)
        av_log(s, AV_LOG_ERROR, "pa_context_get_sink_input_info failed.\n");
    return ret;
}
