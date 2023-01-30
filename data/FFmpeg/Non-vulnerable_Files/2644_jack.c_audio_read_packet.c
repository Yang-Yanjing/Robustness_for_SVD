static int audio_read_packet(AVFormatContext *context, AVPacket *pkt)
{
    JackData *self = context->priv_data;
    struct timespec timeout = {0, 0};
    int test;
    



    if (!self->activated) {
        if (!jack_activate(self->client)) {
            self->activated = 1;
            av_log(context, AV_LOG_INFO,
                   "JACK client registered and activated (rate=%dHz, buffer_size=%d frames)\n",
                   self->sample_rate, self->buffer_size);
        } else {
            av_log(context, AV_LOG_ERROR, "Unable to activate JACK client\n");
            return AVERROR(EIO);
        }
    }
    
    timeout.tv_sec = av_gettime() / 1000000 + 2;
    if (sem_timedwait(&self->packet_count, &timeout)) {
        if (errno == ETIMEDOUT) {
            av_log(context, AV_LOG_ERROR,
                   "Input error: timed out when waiting for JACK process callback output\n");
        } else {
            char errbuf[128];
            int ret = AVERROR(errno);
            av_strerror(ret, errbuf, sizeof(errbuf));
            av_log(context, AV_LOG_ERROR, "Error while waiting for audio packet: %s\n",
                   errbuf);
        }
        if (!self->client)
            av_log(context, AV_LOG_ERROR, "Input error: JACK server is gone\n");
        return AVERROR(EIO);
    }
    if (self->pkt_xrun) {
        av_log(context, AV_LOG_WARNING, "Audio packet xrun\n");
        self->pkt_xrun = 0;
    }
    if (self->jack_xrun) {
        av_log(context, AV_LOG_WARNING, "JACK xrun\n");
        self->jack_xrun = 0;
    }
    
    av_fifo_generic_read(self->filled_pkts, pkt, sizeof(*pkt), NULL);
    if ((test = supply_new_packets(self, context)))
        return test;
    return 0;
}
