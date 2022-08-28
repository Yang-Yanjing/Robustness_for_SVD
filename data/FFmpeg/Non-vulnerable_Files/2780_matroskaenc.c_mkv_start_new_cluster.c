static void mkv_start_new_cluster(AVFormatContext *s, AVPacket *pkt)
{
    MatroskaMuxContext *mkv = s->priv_data;
    AVIOContext *pb;
    if (s->pb->seekable) {
        pb = s->pb;
    } else {
        pb = mkv->dyn_bc;
    }
    av_log(s, AV_LOG_DEBUG,
            "Starting new cluster at offset %" PRIu64 " bytes, "
            "pts %" PRIu64 "dts %" PRIu64 "\n",
            avio_tell(pb), pkt->pts, pkt->dts);
    end_ebml_master(pb, mkv->cluster);
    mkv->cluster_pos = -1;
    if (mkv->dyn_bc)
        mkv_flush_dynbuf(s);
    avio_flush(s->pb);
}
