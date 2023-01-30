static int matroska_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    MatroskaDemuxContext *matroska = s->priv_data;
    while (matroska_deliver_packet(matroska, pkt)) {
        int64_t pos = avio_tell(matroska->ctx->pb);
        if (matroska->done)
            return AVERROR_EOF;
        if (matroska_parse_cluster(matroska) < 0)
            matroska_resync(matroska, pos);
    }
    return 0;
}
