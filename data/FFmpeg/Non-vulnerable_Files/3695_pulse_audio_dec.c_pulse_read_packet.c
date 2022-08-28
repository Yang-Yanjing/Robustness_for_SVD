static int pulse_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    PulseData *pd  = s->priv_data;
    int ret;
    size_t read_length;
    const void *read_data = NULL;
    int64_t dts;
    pa_usec_t latency;
    int negative;
    pa_threaded_mainloop_lock(pd->mainloop);
    CHECK_DEAD_GOTO(pd, ret, unlock_and_fail);
    while (!read_data) {
        int r;
        r = pa_stream_peek(pd->stream, &read_data, &read_length);
        CHECK_SUCCESS_GOTO(ret, r == 0, unlock_and_fail);
        if (read_length <= 0) {
            pa_threaded_mainloop_wait(pd->mainloop);
            CHECK_DEAD_GOTO(pd, ret, unlock_and_fail);
        } else if (!read_data) {
            

            r = pa_stream_drop(pd->stream);
            CHECK_SUCCESS_GOTO(ret, r == 0, unlock_and_fail);
        }
    }
    if (av_new_packet(pkt, read_length) < 0) {
        ret = AVERROR(ENOMEM);
        goto unlock_and_fail;
    }
    dts = av_gettime();
    pa_operation_unref(pa_stream_update_timing_info(pd->stream, NULL, NULL));
    if (pa_stream_get_latency(pd->stream, &latency, &negative) >= 0) {
        enum AVCodecID codec_id =
            s->audio_codec_id == AV_CODEC_ID_NONE ? DEFAULT_CODEC_ID : s->audio_codec_id;
        int frame_size = ((av_get_bits_per_sample(codec_id) >> 3) * pd->channels);
        int frame_duration = read_length / frame_size;
        if (negative) {
            dts += latency;
        } else
            dts -= latency;
        if (pd->wallclock)
            pkt->pts = ff_timefilter_update(pd->timefilter, dts, pd->last_period);
        pd->last_period = frame_duration;
    } else {
        av_log(s, AV_LOG_WARNING, "pa_stream_get_latency() failed\n");
    }
    memcpy(pkt->data, read_data, read_length);
    pa_stream_drop(pd->stream);
    pa_threaded_mainloop_unlock(pd->mainloop);
    return 0;
unlock_and_fail:
    pa_threaded_mainloop_unlock(pd->mainloop);
    return ret;
}
