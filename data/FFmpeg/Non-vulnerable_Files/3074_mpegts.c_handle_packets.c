static int handle_packets(MpegTSContext *ts, int64_t nb_packets)
{
    AVFormatContext *s = ts->stream;
    uint8_t packet[TS_PACKET_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    const uint8_t *data;
    int64_t packet_num;
    int ret = 0;
    if (avio_tell(s->pb) != ts->last_pos) {
        int i;
        av_log(ts->stream, AV_LOG_TRACE, "Skipping after seek\n");
        
        for (i = 0; i < NB_PID_MAX; i++) {
            if (ts->pids[i]) {
                if (ts->pids[i]->type == MPEGTS_PES) {
                    PESContext *pes = ts->pids[i]->u.pes_filter.opaque;
                    av_buffer_unref(&pes->buffer);
                    pes->data_index = 0;
                    pes->state = MPEGTS_SKIP; 
                } else if (ts->pids[i]->type == MPEGTS_SECTION) {
                    ts->pids[i]->u.section_filter.last_ver = -1;
                }
                ts->pids[i]->last_cc = -1;
                ts->pids[i]->last_pcr = -1;
            }
        }
    }
    ts->stop_parse = 0;
    packet_num = 0;
    memset(packet + TS_PACKET_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);
    for (;;) {
        packet_num++;
        if (nb_packets != 0 && packet_num >= nb_packets ||
            ts->stop_parse > 1) {
            ret = AVERROR(EAGAIN);
            break;
        }
        if (ts->stop_parse > 0)
            break;
        ret = read_packet(s, packet, ts->raw_packet_size, &data);
        if (ret != 0)
            break;
        ret = handle_packet(ts, data);
        finished_reading_packet(s, ts->raw_packet_size);
        if (ret != 0)
            break;
    }
    ts->last_pos = avio_tell(s->pb);
    return ret;
}
