static void pulse_stream_writable(pa_stream *stream, size_t nbytes, void *userdata)
{
    AVFormatContext *h = userdata;
    PulseData *s = h->priv_data;
    int64_t val = nbytes;
    if (stream != s->stream)
        return;
    avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_BUFFER_WRITABLE, &val, sizeof(val));
    pa_threaded_mainloop_signal(s->mainloop, 0);
}
