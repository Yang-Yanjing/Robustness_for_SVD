static int read_packet(AVFormatContext *s, uint8_t *buf, int raw_packet_size,
                       const uint8_t **data)
{
    AVIOContext *pb = s->pb;
    int len;
    for (;;) {
        len = ffio_read_indirect(pb, buf, TS_PACKET_SIZE, data);
        if (len != TS_PACKET_SIZE)
            return len < 0 ? len : AVERROR_EOF;
        
        if ((*data)[0] != 0x47) {
            
            uint64_t pos = avio_tell(pb);
            avio_seek(pb, -FFMIN(raw_packet_size, pos), SEEK_CUR);
            if (mpegts_resync(s) < 0)
                return AVERROR(EAGAIN);
            else
                continue;
        } else {
            break;
        }
    }
    return 0;
}
