int avpriv_mpegts_parse_packet(MpegTSContext *ts, AVPacket *pkt,
                               const uint8_t *buf, int len)
{
    int len1;
    len1 = len;
    ts->pkt = pkt;
    for (;;) {
        ts->stop_parse = 0;
        if (len < TS_PACKET_SIZE)
            return AVERROR_INVALIDDATA;
        if (buf[0] != 0x47) {
            buf++;
            len--;
        } else {
            handle_packet(ts, buf);
            buf += TS_PACKET_SIZE;
            len -= TS_PACKET_SIZE;
            if (ts->stop_parse == 1)
                break;
        }
    }
    return len1 - len;
}
