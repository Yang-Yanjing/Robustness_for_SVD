static void matroska_clear_queue(MatroskaDemuxContext *matroska)
{
    matroska->prev_pkt = NULL;
    if (matroska->packets) {
        int n;
        for (n = 0; n < matroska->num_packets; n++) {
            av_free_packet(matroska->packets[n]);
            av_freep(&matroska->packets[n]);
        }
        av_freep(&matroska->packets);
        matroska->num_packets = 0;
    }
}
