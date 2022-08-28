static int pulse_write_packet(AVFormatContext *h, AVPacket *pkt)
{
    PulseData *s = h->priv_data;
    int ret;
    int64_t writable_size;
    if (!pkt)
        return pulse_flash_stream(s);
    if (pkt->dts != AV_NOPTS_VALUE)
        s->timestamp = pkt->dts;
    if (pkt->duration) {
        s->timestamp += pkt->duration;
    } else {
        AVStream *st = h->streams[0];
        AVCodecContext *codec_ctx = st->codec;
        AVRational r = { 1, codec_ctx->sample_rate };
        int64_t samples = pkt->size / (av_get_bytes_per_sample(codec_ctx->sample_fmt) * codec_ctx->channels);
        s->timestamp += av_rescale_q(samples, r, st->time_base);
    }
    pa_threaded_mainloop_lock(s->mainloop);
    if (!PA_STREAM_IS_GOOD(pa_stream_get_state(s->stream))) {
        av_log(s, AV_LOG_ERROR, "PulseAudio stream is in invalid state.\n");
        goto fail;
    }
    while (pa_stream_writable_size(s->stream) < s->minreq) {
        if (s->nonblocking) {
            pa_threaded_mainloop_unlock(s->mainloop);
            return AVERROR(EAGAIN);
        } else
            pa_threaded_mainloop_wait(s->mainloop);
    }
    if ((ret = pa_stream_write(s->stream, pkt->data, pkt->size, NULL, 0, PA_SEEK_RELATIVE)) < 0) {
        av_log(s, AV_LOG_ERROR, "pa_stream_write failed: %s\n", pa_strerror(ret));
        goto fail;
    }
    if ((writable_size = pa_stream_writable_size(s->stream)) >= s->minreq)
        avdevice_dev_to_app_control_message(h, AV_DEV_TO_APP_BUFFER_WRITABLE, &writable_size, sizeof(writable_size));
    pa_threaded_mainloop_unlock(s->mainloop);
    return 0;
  fail:
    pa_threaded_mainloop_unlock(s->mainloop);
    return AVERROR_EXTERNAL;
}
