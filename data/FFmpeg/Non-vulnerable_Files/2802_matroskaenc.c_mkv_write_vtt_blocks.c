static int mkv_write_vtt_blocks(AVFormatContext *s, AVIOContext *pb, AVPacket *pkt)
{
    MatroskaMuxContext *mkv = s->priv_data;
    ebml_master blockgroup;
    int id_size, settings_size, size;
    uint8_t *id, *settings;
    int64_t ts = mkv->tracks[pkt->stream_index].write_dts ? pkt->dts : pkt->pts;
    const int flags = 0;
    id_size = 0;
    id = av_packet_get_side_data(pkt, AV_PKT_DATA_WEBVTT_IDENTIFIER,
                                 &id_size);
    settings_size = 0;
    settings = av_packet_get_side_data(pkt, AV_PKT_DATA_WEBVTT_SETTINGS,
                                       &settings_size);
    size = id_size + 1 + settings_size + 1 + pkt->size;
    av_log(s, AV_LOG_DEBUG, "Writing block at offset %" PRIu64 ", size %d, "
           "pts %" PRId64 ", dts %" PRId64 ", duration %d, flags %d\n",
           avio_tell(pb), size, pkt->pts, pkt->dts, pkt->duration, flags);
    blockgroup = start_ebml_master(pb, MATROSKA_ID_BLOCKGROUP, mkv_blockgroup_size(size));
    put_ebml_id(pb, MATROSKA_ID_BLOCK);
    put_ebml_num(pb, size + 4, 0);
    avio_w8(pb, 0x80 | (pkt->stream_index + 1));     
    avio_wb16(pb, ts - mkv->cluster_pts);
    avio_w8(pb, flags);
    avio_printf(pb, "%.*s\n%.*s\n%.*s", id_size, id, settings_size, settings, pkt->size, pkt->data);
    put_ebml_uint(pb, MATROSKA_ID_BLOCKDURATION, pkt->duration);
    end_ebml_master(pb, blockgroup);
    return pkt->duration;
}
