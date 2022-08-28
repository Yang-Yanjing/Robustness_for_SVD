static inline void put_timestamp(AVIOContext *pb, int id, int64_t timestamp)
{
    avio_w8(pb, (id << 4) |  (((timestamp >> 30) & 0x07)   << 1) | 1);
    avio_wb16(pb, (uint16_t)((((timestamp >> 15) & 0x7fff) << 1) | 1));
    avio_wb16(pb, (uint16_t)((((timestamp)       & 0x7fff) << 1) | 1));
}
