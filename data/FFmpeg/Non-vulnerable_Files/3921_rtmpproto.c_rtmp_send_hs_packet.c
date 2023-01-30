static int rtmp_send_hs_packet(RTMPContext* rt, uint32_t first_int,
                               uint32_t second_int, char *arraydata, int size)
{
    int inoutsize;
    AV_WB32(arraydata, first_int);
    AV_WB32(arraydata + 4, second_int);
    inoutsize = ffurl_write(rt->stream, arraydata,
                            RTMP_HANDSHAKE_PACKET_SIZE);
    if (inoutsize != RTMP_HANDSHAKE_PACKET_SIZE) {
        av_log(rt, AV_LOG_ERROR, "Unable to write answer\n");
        return AVERROR(EIO);
    }
    return 0;
}
