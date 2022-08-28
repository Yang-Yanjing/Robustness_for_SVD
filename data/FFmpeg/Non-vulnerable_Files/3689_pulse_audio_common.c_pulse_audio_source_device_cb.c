static void pulse_audio_source_device_cb(pa_context *c, const pa_source_info *dev,
                                         int eol, void *userdata)
{
    if (!eol)
        pulse_add_detected_device(userdata, dev->name, dev->description);
}
