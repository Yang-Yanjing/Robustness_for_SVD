static void pulse_audio_sink_device_cb(pa_context *ctx, const pa_sink_info *dev,
                                       int eol, void *userdata)
{
    PulseData *s = userdata;
    if (s->ctx != ctx)
        return;
    if (eol) {
        pa_threaded_mainloop_signal(s->mainloop, 0);
    } else {
        if (dev->flags & PA_SINK_FLAT_VOLUME)
            s->base_volume = dev->base_volume;
        else
            s->base_volume = PA_VOLUME_NORM;
        av_log(s, AV_LOG_DEBUG, "base volume: %u\n", s->base_volume);
    }
}
