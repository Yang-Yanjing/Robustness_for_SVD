static int pulse_control_message(AVFormatContext *h, int type,
                                 void *data, size_t data_size)
{
    PulseData *s = h->priv_data;
    int ret;
    switch(type) {
    case AV_APP_TO_DEV_PAUSE:
        return pulse_set_pause(s, 1);
    case AV_APP_TO_DEV_PLAY:
        return pulse_set_pause(s, 0);
    case AV_APP_TO_DEV_TOGGLE_PAUSE:
        return pulse_set_pause(s, !pa_stream_is_corked(s->stream));
    case AV_APP_TO_DEV_MUTE:
        if (!s->mute) {
            s->mute = 1;
            return pulse_set_mute(s);
        }
        return 0;
    case AV_APP_TO_DEV_UNMUTE:
        if (s->mute) {
            s->mute = 0;
            return pulse_set_mute(s);
        }
        return 0;
    case AV_APP_TO_DEV_TOGGLE_MUTE:
        s->mute = !s->mute;
        return pulse_set_mute(s);
    case AV_APP_TO_DEV_SET_VOLUME:
        return pulse_set_volume(s, *(double *)data);
    case AV_APP_TO_DEV_GET_VOLUME:
        s->last_volume = PA_VOLUME_INVALID;
        pa_threaded_mainloop_lock(s->mainloop);
        ret = pulse_update_sink_input_info(h);
        pa_threaded_mainloop_unlock(s->mainloop);
        return ret;
    case AV_APP_TO_DEV_GET_MUTE:
        s->mute = -1;
        pa_threaded_mainloop_lock(s->mainloop);
        ret = pulse_update_sink_input_info(h);
        pa_threaded_mainloop_unlock(s->mainloop);
        return ret;
    default:
        break;
    }
    return AVERROR(ENOSYS);
}
