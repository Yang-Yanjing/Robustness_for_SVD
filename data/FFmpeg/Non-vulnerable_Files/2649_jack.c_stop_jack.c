static void stop_jack(JackData *self)
{
    if (self->client) {
        if (self->activated)
            jack_deactivate(self->client);
        jack_client_close(self->client);
    }
    sem_destroy(&self->packet_count);
    free_pkt_fifo(&self->new_pkts);
    free_pkt_fifo(&self->filled_pkts);
    av_freep(&self->ports);
    ff_timefilter_destroy(self->timefilter);
}
