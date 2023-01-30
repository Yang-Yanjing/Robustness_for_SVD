static int mkv_write_flush_packet(AVFormatContext *s, AVPacket *pkt)
{
    MatroskaMuxContext *mkv = s->priv_data;
    AVIOContext *pb;
    if (s->pb->seekable)
        pb = s->pb;
    else
        pb = mkv->dyn_bc;
    if (!pkt) {
        if (mkv->cluster_pos != -1) {
            av_log(s, AV_LOG_DEBUG,
                   "Flushing cluster at offset %" PRIu64 " bytes\n",
                   avio_tell(pb));
            end_ebml_master(pb, mkv->cluster);
            mkv->cluster_pos = -1;
            if (mkv->dyn_bc)
                mkv_flush_dynbuf(s);
            avio_flush(s->pb);
        }
        return 1;
    }
    return mkv_write_packet(s, pkt);
}
