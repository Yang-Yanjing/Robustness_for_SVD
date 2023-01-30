static av_cold int pulse_close(AVFormatContext *s)
{
    PulseData *pd = s->priv_data;
    if (pd->mainloop)
        pa_threaded_mainloop_stop(pd->mainloop);
    if (pd->stream)
        pa_stream_unref(pd->stream);
    pd->stream = NULL;
    if (pd->context) {
        pa_context_disconnect(pd->context);
        pa_context_unref(pd->context);
    }
    pd->context = NULL;
    if (pd->mainloop)
        pa_threaded_mainloop_free(pd->mainloop);
    pd->mainloop = NULL;
    ff_timefilter_destroy(pd->timefilter);
    pd->timefilter = NULL;
    return 0;
}
