static void pulse_audio_sink_device_cb(pa_context *c, const pa_sink_info *dev,
                                       int eol, void *userdata)
{
    if (!eol)
        pulse_add_detected_device(userdata, dev->name, dev->description);
}
