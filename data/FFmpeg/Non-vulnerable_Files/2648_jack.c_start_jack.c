static int start_jack(AVFormatContext *context)
{
    JackData *self = context->priv_data;
    jack_status_t status;
    int i, test;
    
    self->client = jack_client_open(context->filename, JackNullOption, &status);
    if (!self->client) {
        av_log(context, AV_LOG_ERROR, "Unable to register as a JACK client\n");
        return AVERROR(EIO);
    }
    sem_init(&self->packet_count, 0, 0);
    self->sample_rate = jack_get_sample_rate(self->client);
    self->ports       = av_malloc_array(self->nports, sizeof(*self->ports));
    if (!self->ports)
        return AVERROR(ENOMEM);
    self->buffer_size = jack_get_buffer_size(self->client);
    
    for (i = 0; i < self->nports; i++) {
        char str[16];
        snprintf(str, sizeof(str), "input_%d", i + 1);
        self->ports[i] = jack_port_register(self->client, str,
                                            JACK_DEFAULT_AUDIO_TYPE,
                                            JackPortIsInput, 0);
        if (!self->ports[i]) {
            av_log(context, AV_LOG_ERROR, "Unable to register port %s:%s\n",
                   context->filename, str);
            jack_client_close(self->client);
            return AVERROR(EIO);
        }
    }
    
    jack_set_process_callback(self->client, process_callback, self);
    jack_on_shutdown(self->client, shutdown_callback, self);
    jack_set_xrun_callback(self->client, xrun_callback, self);
    
    self->timefilter  = ff_timefilter_new (1.0 / self->sample_rate, self->buffer_size, 1.5);
    if (!self->timefilter) {
        jack_client_close(self->client);
        return AVERROR(ENOMEM);
    }
    
    self->filled_pkts = av_fifo_alloc_array(FIFO_PACKETS_NUM, sizeof(AVPacket));
    
    self->new_pkts    = av_fifo_alloc_array((FIFO_PACKETS_NUM + 1), sizeof(AVPacket));
    if (!self->new_pkts) {
        jack_client_close(self->client);
        return AVERROR(ENOMEM);
    }
    if ((test = supply_new_packets(self, context))) {
        jack_client_close(self->client);
        return test;
    }
    return 0;
}
