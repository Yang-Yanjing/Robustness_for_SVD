static void pulse_audio_sink_input_cb(pa_context *ctx, const pa_sink_input_info *i,
                                      int eol, void *userdata)
{
    AVFormatContext *h = userdata;
    PulseData *s = h->priv_data;
    if (s->ctx != ctx)
        return;
    if (!eol) {
        double val;
        pa_volume_t vol = pa_cvolume_avg(&i->volume);
        if (s->mute < 0 || (s->mute && !i->mute) || (!s->mute && i->mute)) {
            s->mute = i->mute;
            avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_MUTE_STATE_CHANGED, &s->mute, sizeof(s->mute));
        }
        vol = pa_sw_volume_divide(vol, s->base_volume);
        if (s->last_volume != vol) {
            val = (double)vol / PA_VOLUME_NORM;
            avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_VOLUME_LEVEL_CHANGED, &val, sizeof(val));
            s->last_volume = vol;
        }
    }
}
