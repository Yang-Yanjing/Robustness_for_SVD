static int supply_new_packets(JackData *self, AVFormatContext *context)
{
    AVPacket pkt;
    int test, pkt_size = self->buffer_size * self->nports * sizeof(float);
    


    while (av_fifo_space(self->new_pkts) >= sizeof(pkt)) {
        if ((test = av_new_packet(&pkt, pkt_size)) < 0) {
            av_log(context, AV_LOG_ERROR, "Could not create packet of size %d\n", pkt_size);
            return test;
        }
        av_fifo_generic_write(self->new_pkts, &pkt, sizeof(pkt), NULL);
    }
    return 0;
}
