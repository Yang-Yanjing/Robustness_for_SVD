static int ebml_read_num(MatroskaDemuxContext *matroska, AVIOContext *pb,
                         int max_size, uint64_t *number)
{
    int read = 1, n = 1;
    uint64_t total = 0;
    


    if (!(total = avio_r8(pb))) {
        
        if (!avio_feof(pb)) {
            int64_t pos = avio_tell(pb);
            av_log(matroska->ctx, AV_LOG_ERROR,
                   "Read error at pos. %"PRIu64" (0x%"PRIx64")\n",
                   pos, pos);
            return pb->error ? pb->error : AVERROR(EIO);
        }
        return AVERROR_EOF;
    }
    
    read = 8 - ff_log2_tab[total];
    if (read > max_size) {
        int64_t pos = avio_tell(pb) - 1;
        av_log(matroska->ctx, AV_LOG_ERROR,
               "Invalid EBML number size tag 0x%02x at pos %"PRIu64" (0x%"PRIx64")\n",
               (uint8_t) total, pos, pos);
        return AVERROR_INVALIDDATA;
    }
    
    total ^= 1 << ff_log2_tab[total];
    while (n++ < read)
        total = (total << 8) | avio_r8(pb);
    *number = total;
    return read;
}
