static av_cold int pulse_write_header(AVFormatContext *h)
{
    PulseData *s = h->priv_data;
    AVStream *st = NULL;
    int ret;
    pa_sample_spec sample_spec;
    pa_buffer_attr buffer_attributes = { -1, -1, -1, -1, -1 };
    pa_channel_map channel_map;
    pa_mainloop_api *mainloop_api;
    const char *stream_name = s->stream_name;
    static const pa_stream_flags_t stream_flags = PA_STREAM_INTERPOLATE_TIMING |
                                                  PA_STREAM_AUTO_TIMING_UPDATE |
                                                  PA_STREAM_NOT_MONOTONIC;
    if (h->nb_streams != 1 || h->streams[0]->codec->codec_type != AVMEDIA_TYPE_AUDIO) {
        av_log(s, AV_LOG_ERROR, "Only a single audio stream is supported.\n");
        return AVERROR(EINVAL);
    }
    st = h->streams[0];
    if (!stream_name) {
        if (h->filename[0])
            stream_name = h->filename;
        else
            stream_name = "Playback";
    }
    s->nonblocking = (h->flags & AVFMT_FLAG_NONBLOCK);
    if (s->buffer_duration) {
        int64_t bytes = s->buffer_duration;
        bytes *= st->codec->channels * st->codec->sample_rate *
                 av_get_bytes_per_sample(st->codec->sample_fmt);
        bytes /= 1000;
        buffer_attributes.tlength = FFMAX(s->buffer_size, av_clip64(bytes, 0, UINT32_MAX - 1));
        av_log(s, AV_LOG_DEBUG,
               "Buffer duration: %ums recalculated into %"PRId64" bytes buffer.\n",
               s->buffer_duration, bytes);
        av_log(s, AV_LOG_DEBUG, "Real buffer length is %u bytes\n", buffer_attributes.tlength);
    } else if (s->buffer_size)
        buffer_attributes.tlength = s->buffer_size;
    if (s->prebuf)
        buffer_attributes.prebuf = s->prebuf;
    if (s->minreq)
        buffer_attributes.minreq = s->minreq;
    sample_spec.format = ff_codec_id_to_pulse_format(st->codec->codec_id);
    sample_spec.rate = st->codec->sample_rate;
    sample_spec.channels = st->codec->channels;
    if (!pa_sample_spec_valid(&sample_spec)) {
        av_log(s, AV_LOG_ERROR, "Invalid sample spec.\n");
        return AVERROR(EINVAL);
    }
    if (sample_spec.channels == 1) {
        channel_map.channels = 1;
        channel_map.map[0] = PA_CHANNEL_POSITION_MONO;
    } else if (st->codec->channel_layout) {
        if (av_get_channel_layout_nb_channels(st->codec->channel_layout) != st->codec->channels)
            return AVERROR(EINVAL);
        pulse_map_channels_to_pulse(st->codec->channel_layout, &channel_map);
        
        if (channel_map.channels != sample_spec.channels) {
            av_log(s, AV_LOG_WARNING, "Unknown channel. Using defaul channel map.\n");
            channel_map.channels = 0;
        }
    } else
        channel_map.channels = 0;
    if (!channel_map.channels)
        av_log(s, AV_LOG_WARNING, "Using PulseAudio's default channel map.\n");
    else if (!pa_channel_map_valid(&channel_map)) {
        av_log(s, AV_LOG_ERROR, "Invalid channel map.\n");
        return AVERROR(EINVAL);
    }
    
    s->mainloop = pa_threaded_mainloop_new();
    if (!s->mainloop) {
        av_log(s, AV_LOG_ERROR, "Cannot create threaded mainloop.\n");
        return AVERROR(ENOMEM);
    }
    if ((ret = pa_threaded_mainloop_start(s->mainloop)) < 0) {
        av_log(s, AV_LOG_ERROR, "Cannot start threaded mainloop: %s.\n", pa_strerror(ret));
        pa_threaded_mainloop_free(s->mainloop);
        s->mainloop = NULL;
        return AVERROR_EXTERNAL;
    }
    pa_threaded_mainloop_lock(s->mainloop);
    mainloop_api = pa_threaded_mainloop_get_api(s->mainloop);
    if (!mainloop_api) {
        av_log(s, AV_LOG_ERROR, "Cannot get mainloop API.\n");
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    s->ctx = pa_context_new(mainloop_api, s->name);
    if (!s->ctx) {
        av_log(s, AV_LOG_ERROR, "Cannot create context.\n");
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    pa_context_set_state_callback(s->ctx, pulse_context_state, s);
    pa_context_set_subscribe_callback(s->ctx, pulse_event, h);
    if ((ret = pa_context_connect(s->ctx, s->server, 0, NULL)) < 0) {
        av_log(s, AV_LOG_ERROR, "Cannot connect context: %s.\n", pa_strerror(ret));
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    if ((ret = pulse_context_wait(s)) < 0) {
        av_log(s, AV_LOG_ERROR, "Context failed.\n");
        goto fail;
    }
    s->stream = pa_stream_new(s->ctx, stream_name, &sample_spec,
                              channel_map.channels ? &channel_map : NULL);
    if ((ret = pulse_update_sink_info(h)) < 0) {
        av_log(s, AV_LOG_ERROR, "Updating sink info failed.\n");
        goto fail;
    }
    if (!s->stream) {
        av_log(s, AV_LOG_ERROR, "Cannot create stream.\n");
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    pa_stream_set_state_callback(s->stream, pulse_stream_state, s);
    pa_stream_set_write_callback(s->stream, pulse_stream_writable, h);
    pa_stream_set_overflow_callback(s->stream, pulse_overflow, h);
    pa_stream_set_underflow_callback(s->stream, pulse_underflow, h);
    if ((ret = pa_stream_connect_playback(s->stream, s->device, &buffer_attributes,
                                          stream_flags, NULL, NULL)) < 0) {
        av_log(s, AV_LOG_ERROR, "pa_stream_connect_playback failed: %s.\n", pa_strerror(ret));
        ret = AVERROR_EXTERNAL;
        goto fail;
    }
    if ((ret = pulse_stream_wait(s)) < 0) {
        av_log(s, AV_LOG_ERROR, "Stream failed.\n");
        goto fail;
    }
    
    buffer_attributes = *pa_stream_get_buffer_attr(s->stream);
    s->buffer_size = buffer_attributes.tlength;
    s->prebuf = buffer_attributes.prebuf;
    s->minreq = buffer_attributes.minreq;
    av_log(s, AV_LOG_DEBUG, "Real buffer attributes: size: %d, prebuf: %d, minreq: %d\n",
           s->buffer_size, s->prebuf, s->minreq);
    pa_threaded_mainloop_unlock(s->mainloop);
    if ((ret = pulse_subscribe_events(s)) < 0) {
        av_log(s, AV_LOG_ERROR, "Event subscription failed.\n");
        
        pa_threaded_mainloop_lock(s->mainloop);
        goto fail;
    }
    
    s->mute = -1;
    s->last_volume = PA_VOLUME_INVALID;
    pa_threaded_mainloop_lock(s->mainloop);
    if ((ret = pulse_update_sink_input_info(h)) < 0) {
        av_log(s, AV_LOG_ERROR, "Updating sink input info failed.\n");
        goto fail;
    }
    pa_threaded_mainloop_unlock(s->mainloop);
    avpriv_set_pts_info(st, 64, 1, 1000000);  
    return 0;
  fail:
    pa_threaded_mainloop_unlock(s->mainloop);
    pulse_write_trailer(h);
    return ret;
}
