static av_cold int pulse_read_header(AVFormatContext *s)
{
    PulseData *pd = s->priv_data;
    AVStream *st;
    char *device = NULL;
    int ret;
    enum AVCodecID codec_id =
        s->audio_codec_id == AV_CODEC_ID_NONE ? DEFAULT_CODEC_ID : s->audio_codec_id;
    const pa_sample_spec ss = { ff_codec_id_to_pulse_format(codec_id),
                                pd->sample_rate,
                                pd->channels };
    pa_buffer_attr attr = { -1 };
    st = avformat_new_stream(s, NULL);
    if (!st) {
        av_log(s, AV_LOG_ERROR, "Cannot add stream\n");
        return AVERROR(ENOMEM);
    }
    attr.fragsize = pd->fragment_size;
    if (s->filename[0] != '\0' && strcmp(s->filename, "default"))
        device = s->filename;
    if (!(pd->mainloop = pa_threaded_mainloop_new())) {
        pulse_close(s);
        return AVERROR_EXTERNAL;
    }
    if (!(pd->context = pa_context_new(pa_threaded_mainloop_get_api(pd->mainloop), pd->name))) {
        pulse_close(s);
        return AVERROR_EXTERNAL;
    }
    pa_context_set_state_callback(pd->context, context_state_cb, pd);
    if (pa_context_connect(pd->context, pd->server, 0, NULL) < 0) {
        pulse_close(s);
        return AVERROR(pa_context_errno(pd->context));
    }
    pa_threaded_mainloop_lock(pd->mainloop);
    if (pa_threaded_mainloop_start(pd->mainloop) < 0) {
        ret = -1;
        goto unlock_and_fail;
    }
    for (;;) {
        pa_context_state_t state;
        state = pa_context_get_state(pd->context);
        if (state == PA_CONTEXT_READY)
            break;
        if (!PA_CONTEXT_IS_GOOD(state)) {
            ret = AVERROR(pa_context_errno(pd->context));
            goto unlock_and_fail;
        }
        
        pa_threaded_mainloop_wait(pd->mainloop);
    }
    if (!(pd->stream = pa_stream_new(pd->context, pd->stream_name, &ss, NULL))) {
        ret = AVERROR(pa_context_errno(pd->context));
        goto unlock_and_fail;
    }
    pa_stream_set_state_callback(pd->stream, stream_state_cb, pd);
    pa_stream_set_read_callback(pd->stream, stream_request_cb, pd);
    pa_stream_set_write_callback(pd->stream, stream_request_cb, pd);
    pa_stream_set_latency_update_callback(pd->stream, stream_latency_update_cb, pd);
    ret = pa_stream_connect_record(pd->stream, device, &attr,
                                    PA_STREAM_INTERPOLATE_TIMING
                                    |PA_STREAM_ADJUST_LATENCY
                                    |PA_STREAM_AUTO_TIMING_UPDATE);
    if (ret < 0) {
        ret = AVERROR(pa_context_errno(pd->context));
        goto unlock_and_fail;
    }
    for (;;) {
        pa_stream_state_t state;
        state = pa_stream_get_state(pd->stream);
        if (state == PA_STREAM_READY)
            break;
        if (!PA_STREAM_IS_GOOD(state)) {
            ret = AVERROR(pa_context_errno(pd->context));
            goto unlock_and_fail;
        }
        
        pa_threaded_mainloop_wait(pd->mainloop);
    }
    pa_threaded_mainloop_unlock(pd->mainloop);
    
    st->codec->codec_type  = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_id    = codec_id;
    st->codec->sample_rate = pd->sample_rate;
    st->codec->channels    = pd->channels;
    avpriv_set_pts_info(st, 64, 1, 1000000);  
    pd->timefilter = ff_timefilter_new(1000000.0 / pd->sample_rate,
                                       1000, 1.5E-6);
    if (!pd->timefilter) {
        pulse_close(s);
        return AVERROR(ENOMEM);
    }
    return 0;
unlock_and_fail:
    pa_threaded_mainloop_unlock(pd->mainloop);
    pulse_close(s);
    return ret;
}
