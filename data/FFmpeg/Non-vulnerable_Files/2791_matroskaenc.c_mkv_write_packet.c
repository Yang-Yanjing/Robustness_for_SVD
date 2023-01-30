static int mkv_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    MatroskaMuxContext *mkv = s->priv_data;
    int codec_type          = s->streams[pkt->stream_index]->codec->codec_type;
    int keyframe            = !!(pkt->flags & AV_PKT_FLAG_KEY);
    int cluster_size;
    int64_t cluster_time;
    int ret;
    int start_new_cluster;
    if (mkv->tracks[pkt->stream_index].write_dts)
        cluster_time = pkt->dts - mkv->cluster_pts;
    else
        cluster_time = pkt->pts - mkv->cluster_pts;
    cluster_time += mkv->tracks[pkt->stream_index].ts_offset;
    
    
    if (s->pb->seekable) {
        cluster_size = avio_tell(s->pb) - mkv->cluster_pos;
    } else {
        cluster_size = avio_tell(mkv->dyn_bc);
    }
    if (mkv->is_dash && codec_type == AVMEDIA_TYPE_VIDEO) {
        
        
        
        start_new_cluster = keyframe;
    } else if (mkv->is_dash && codec_type == AVMEDIA_TYPE_AUDIO &&
               (mkv->cluster_pos == -1 ||
                cluster_time > mkv->cluster_time_limit)) {
        
        start_new_cluster = 1;
    } else if (!mkv->is_dash &&
               (cluster_size > mkv->cluster_size_limit ||
                cluster_time > mkv->cluster_time_limit ||
                (codec_type == AVMEDIA_TYPE_VIDEO && keyframe &&
                 cluster_size > 4 * 1024))) {
        start_new_cluster = 1;
    } else {
        start_new_cluster = 0;
    }
    if (mkv->cluster_pos != -1 && start_new_cluster) {
        mkv_start_new_cluster(s, pkt);
    }
    
    if (mkv->cur_audio_pkt.size > 0) {
        
        ret = mkv_write_packet_internal(s, &mkv->cur_audio_pkt,
                                        mkv->is_dash ? start_new_cluster : 0);
        av_free_packet(&mkv->cur_audio_pkt);
        if (ret < 0) {
            av_log(s, AV_LOG_ERROR,
                   "Could not write cached audio packet ret:%d\n", ret);
            return ret;
        }
    }
    
    
    if (codec_type == AVMEDIA_TYPE_AUDIO) {
        mkv->cur_audio_pkt = *pkt;
        if (pkt->buf) {
            mkv->cur_audio_pkt.buf = av_buffer_ref(pkt->buf);
            ret = mkv->cur_audio_pkt.buf ? 0 : AVERROR(ENOMEM);
        } else
            ret = av_dup_packet(&mkv->cur_audio_pkt);
        if (mkv->cur_audio_pkt.side_data_elems > 0) {
            ret = av_copy_packet_side_data(&mkv->cur_audio_pkt, &mkv->cur_audio_pkt);
        }
    } else
        ret = mkv_write_packet_internal(s, pkt, 0);
    return ret;
}
