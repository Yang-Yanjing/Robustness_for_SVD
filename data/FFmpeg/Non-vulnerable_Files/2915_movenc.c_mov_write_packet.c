static int mov_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    if (!pkt) {
        mov_flush_fragment(s);
        return 1;
    } else {
        int i;
        MOVMuxContext *mov = s->priv_data;
        if (!pkt->size) return 0; 
        


















        for (i = 0; i < mov->nb_streams; i++) {
            MOVTrack *trk = &mov->tracks[i];
            int ret;
            if (trk->enc->codec_id == AV_CODEC_ID_MOV_TEXT &&
                trk->track_duration < pkt->dts &&
                (trk->entry == 0 || !trk->last_sample_is_subtitle_end)) {
                ret = mov_write_subtitle_end_packet(s, i, trk->track_duration);
                if (ret < 0) return ret;
                trk->last_sample_is_subtitle_end = 1;
            }
        }
        return mov_write_single_packet(s, pkt);
    }
}
