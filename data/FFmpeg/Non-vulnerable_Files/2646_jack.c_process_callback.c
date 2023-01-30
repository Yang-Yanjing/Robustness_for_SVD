static int process_callback(jack_nframes_t nframes, void *arg)
{
    

    int i, j;
    JackData *self = arg;
    float * buffer;
    jack_nframes_t latency, cycle_delay;
    AVPacket pkt;
    float *pkt_data;
    double cycle_time;
    if (!self->client)
        return 0;
    
    cycle_delay = jack_frames_since_cycle_start(self->client);
    
    cycle_time = ff_timefilter_update(self->timefilter,
                                      av_gettime() / 1000000.0 - (double) cycle_delay / self->sample_rate,
                                      self->buffer_size);
    
    if ((av_fifo_size(self->new_pkts) < sizeof(pkt)) || (av_fifo_space(self->filled_pkts) < sizeof(pkt))) {
        self->pkt_xrun = 1;
        return 0;
    }
    
    av_fifo_generic_read(self->new_pkts, &pkt, sizeof(pkt), NULL);
    pkt_data  = (float *) pkt.data;
    latency   = 0;
    
    for (i = 0; i < self->nports; i++) {
    #if HAVE_JACK_PORT_GET_LATENCY_RANGE
        jack_latency_range_t range;
        jack_port_get_latency_range(self->ports[i], JackCaptureLatency, &range);
        latency += range.max;
    #else
        latency += jack_port_get_total_latency(self->client, self->ports[i]);
    #endif
        buffer = jack_port_get_buffer(self->ports[i], self->buffer_size);
        for (j = 0; j < self->buffer_size; j++)
            pkt_data[j * self->nports + i] = buffer[j];
    }
    
    pkt.pts = (cycle_time - (double) latency / (self->nports * self->sample_rate)) * 1000000.0;
    
    av_fifo_generic_write(self->filled_pkts, &pkt, sizeof(pkt), NULL);
    sem_post(&self->packet_count);
    return 0;
}
