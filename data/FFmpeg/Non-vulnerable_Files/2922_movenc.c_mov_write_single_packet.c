static int mov_write_single_packet(AVFormatContext *s, AVPacket *pkt)
{
        MOVMuxContext *mov = s->priv_data;
        MOVTrack *trk = &mov->tracks[pkt->stream_index];
        AVCodecContext *enc = trk->enc;
        int64_t frag_duration = 0;
        int size = pkt->size;
        if (!pkt->size)
            return 0;             
        if (mov->flags & FF_MOV_FLAG_FRAG_DISCONT) {
            int i;
            for (i = 0; i < s->nb_streams; i++)
                mov->tracks[i].frag_discont = 1;
            mov->flags &= ~FF_MOV_FLAG_FRAG_DISCONT;
        }
        if (trk->entry && pkt->stream_index < s->nb_streams)
            frag_duration = av_rescale_q(pkt->dts - trk->cluster[0].dts,
                                         s->streams[pkt->stream_index]->time_base,
                                         AV_TIME_BASE_Q);
        if ((mov->max_fragment_duration &&
             frag_duration >= mov->max_fragment_duration) ||
             (mov->max_fragment_size && mov->mdat_size + size >= mov->max_fragment_size) ||
             (mov->flags & FF_MOV_FLAG_FRAG_KEYFRAME &&
              enc->codec_type == AVMEDIA_TYPE_VIDEO &&
              trk->entry && pkt->flags & AV_PKT_FLAG_KEY)) {
            if (frag_duration >= mov->min_fragment_duration) {
                
                
                
                
                trk->track_duration = pkt->dts - trk->start_dts;
                trk->end_pts = pkt->pts;
                mov_auto_flush_fragment(s);
            }
        }
        return ff_mov_write_packet(s, pkt);
}
