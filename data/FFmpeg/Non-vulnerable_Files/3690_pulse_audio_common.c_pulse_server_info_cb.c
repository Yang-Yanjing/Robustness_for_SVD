static void pulse_server_info_cb(pa_context *c, const pa_server_info *i, void *userdata)
{
    PulseAudioDeviceList *info = userdata;
    if (info->output)
        info->default_device = av_strdup(i->default_sink_name);
    else
        info->default_device = av_strdup(i->default_source_name);
    if (!info->default_device)
        info->error_code = AVERROR(ENOMEM);
}
