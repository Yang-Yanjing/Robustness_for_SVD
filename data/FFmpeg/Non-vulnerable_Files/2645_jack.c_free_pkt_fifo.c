static void free_pkt_fifo(AVFifoBuffer **fifo)
{
    AVPacket pkt;
    while (av_fifo_size(*fifo)) {
        av_fifo_generic_read(*fifo, &pkt, sizeof(pkt), NULL);
        av_free_packet(&pkt);
    }
    av_fifo_freep(fifo);
}
