static int64_t klv_decode_ber_length(AVIOContext *pb)
{
    uint64_t size = avio_r8(pb);
    if (size & 0x80) { 
        int bytes_num = size & 0x7f;
        
        if (bytes_num > 8)
            return AVERROR_INVALIDDATA;
        size = 0;
        while (bytes_num--)
            size = size << 8 | avio_r8(pb);
    }
    return size;
}
