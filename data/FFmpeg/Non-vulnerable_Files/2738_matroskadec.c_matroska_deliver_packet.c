static int matroska_deliver_packet(MatroskaDemuxContext *matroska,
                                   AVPacket *pkt)
{
    if (matroska->num_packets > 0) {
        memcpy(pkt, matroska->packets[0], sizeof(AVPacket));
        av_freep(&matroska->packets[0]);
        if (matroska->num_packets > 1) {
            void *newpackets;
            memmove(&matroska->packets[0], &matroska->packets[1],
                    (matroska->num_packets - 1) * sizeof(AVPacket *));
            newpackets = av_realloc(matroska->packets,
                                    (matroska->num_packets - 1) *
                                    sizeof(AVPacket *));
            if (newpackets)
                matroska->packets = newpackets;
        } else {
            av_freep(&matroska->packets);
            matroska->prev_pkt = NULL;
        }
        matroska->num_packets--;
        return 0;
    }
    return -1;
}
