static int webm_clusters_start_with_keyframe(AVFormatContext *s)
{
    MatroskaDemuxContext *matroska = s->priv_data;
    int64_t cluster_pos, before_pos;
    int index, rv = 1;
    if (s->streams[0]->nb_index_entries <= 0) return 0;
    
    index = av_index_search_timestamp(s->streams[0], 0, 0);
    if (index < 0)  return 0;
    cluster_pos = s->streams[0]->index_entries[index].pos;
    before_pos = avio_tell(s->pb);
    while (1) {
        int64_t cluster_id = 0, cluster_length = 0;
        AVPacket *pkt;
        avio_seek(s->pb, cluster_pos, SEEK_SET);
        
        ebml_read_num(matroska, matroska->ctx->pb, 4, &cluster_id);
        ebml_read_length(matroska, matroska->ctx->pb, &cluster_length);
        if (cluster_id != 0xF43B675) { 
            break;
        }
        avio_seek(s->pb, cluster_pos, SEEK_SET);
        matroska->current_id = 0;
        matroska_clear_queue(matroska);
        if (matroska_parse_cluster(matroska) < 0 ||
            matroska->num_packets <= 0) {
            break;
        }
        pkt = matroska->packets[0];
        cluster_pos += cluster_length + 12; 
        if (!(pkt->flags & AV_PKT_FLAG_KEY)) {
            rv = 0;
            break;
        }
    }
    avio_seek(s->pb, before_pos, SEEK_SET);
    return rv;
}
