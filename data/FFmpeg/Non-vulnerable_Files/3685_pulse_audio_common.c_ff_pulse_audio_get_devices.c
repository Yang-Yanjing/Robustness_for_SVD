int ff_pulse_audio_get_devices(AVDeviceInfoList *devices, const char *server, int output)
{
    pa_mainloop *pa_ml = NULL;
    pa_operation *pa_op = NULL;
    pa_context *pa_ctx = NULL;
    enum pa_operation_state op_state;
    PulseAudioDeviceList dev_list = { 0 };
    int i;
    dev_list.output = output;
    dev_list.devices = devices;
    if (!devices)
        return AVERROR(EINVAL);
    devices->nb_devices = 0;
    devices->devices = NULL;
    if ((dev_list.error_code = ff_pulse_audio_connect_context(&pa_ml, &pa_ctx, server, "Query devices")) < 0)
        goto fail;
    if (output)
        pa_op = pa_context_get_sink_info_list(pa_ctx, pulse_audio_sink_device_cb, &dev_list);
    else
        pa_op = pa_context_get_source_info_list(pa_ctx, pulse_audio_source_device_cb, &dev_list);
    while ((op_state = pa_operation_get_state(pa_op)) == PA_OPERATION_RUNNING)
        pa_mainloop_iterate(pa_ml, 1, NULL);
    if (op_state != PA_OPERATION_DONE)
        dev_list.error_code = AVERROR_EXTERNAL;
    pa_operation_unref(pa_op);
    if (dev_list.error_code < 0)
        goto fail;
    pa_op = pa_context_get_server_info(pa_ctx, pulse_server_info_cb, &dev_list);
    while ((op_state = pa_operation_get_state(pa_op)) == PA_OPERATION_RUNNING)
        pa_mainloop_iterate(pa_ml, 1, NULL);
    if (op_state != PA_OPERATION_DONE)
        dev_list.error_code = AVERROR_EXTERNAL;
    pa_operation_unref(pa_op);
    if (dev_list.error_code < 0)
        goto fail;
    devices->default_device = -1;
    for (i = 0; i < devices->nb_devices; i++) {
        if (!strcmp(devices->devices[i]->device_name, dev_list.default_device)) {
            devices->default_device = i;
            break;
        }
    }
  fail:
    av_free(dev_list.default_device);
    ff_pulse_audio_disconnect_context(&pa_ml, &pa_ctx);
    return dev_list.error_code;
}
