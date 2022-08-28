static uint64_t get_fourcc(AVIOContext *bc)
{
    unsigned int len = ffio_read_varlen(bc);
    if (len == 2)
        return avio_rl16(bc);
    else if (len == 4)
        return avio_rl32(bc);
    else {
        av_log(NULL, AV_LOG_ERROR, "Unsupported fourcc length %d\n", len);
        return -1;
    }
}
