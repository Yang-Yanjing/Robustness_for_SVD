static int klv_encode_ber_length(AVIOContext *pb, uint64_t len)
{
    
    int size;
    if (len < 128) {
        
        avio_w8(pb, len);
        return 1;
    }
    size = (av_log2(len) >> 3) + 1;
    
    avio_w8(pb, 0x80 + size);
    while(size) {
        size--;
        avio_w8(pb, len >> 8 * size & 0xff);
    }
    return 0;
}
