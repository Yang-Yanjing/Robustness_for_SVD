static int hls_read_seek(AVFormatContext *s, int stream_index,
                               int64_t timestamp, int flags)
{
    HLSContext *c = s->priv_data;
    struct playlist *seek_pls = NULL;
    int i, seq_no;
    int64_t first_timestamp, seek_timestamp, duration;
    if ((flags & AVSEEK_FLAG_BYTE) ||
        !(c->variants[0]->playlists[0]->finished || c->variants[0]->playlists[0]->type == PLS_TYPE_EVENT))
        return AVERROR(ENOSYS);
    first_timestamp = c->first_timestamp == AV_NOPTS_VALUE ?
                      0 : c->first_timestamp;
    seek_timestamp = av_rescale_rnd(timestamp, AV_TIME_BASE,
                                    s->streams[stream_index]->time_base.den,
                                    flags & AVSEEK_FLAG_BACKWARD ?
                                    AV_ROUND_DOWN : AV_ROUND_UP);
    duration = s->duration == AV_NOPTS_VALUE ?
               0 : s->duration;
    if (0 < duration && duration < seek_timestamp - first_timestamp)
        return AVERROR(EIO);
    
    for (i = 0; i < c->n_playlists; i++) {
        struct playlist *pls = c->playlists[i];
        if (stream_index >= pls->stream_offset &&
            stream_index - pls->stream_offset < pls->ctx->nb_streams) {
            seek_pls = pls;
            break;
        }
    }
    

    if (!seek_pls || !find_timestamp_in_playlist(c, seek_pls, seek_timestamp, &seq_no))
        return AVERROR(EIO);
    
    seek_pls->cur_seq_no = seq_no;
    seek_pls->seek_stream_index = stream_index - seek_pls->stream_offset;
    for (i = 0; i < c->n_playlists; i++) {
        
        struct playlist *pls = c->playlists[i];
        if (pls->input) {
            ffurl_close(pls->input);
            pls->input = NULL;
        }
        av_free_packet(&pls->pkt);
        reset_packet(&pls->pkt);
        pls->pb.eof_reached = 0;
        
        pls->pb.buf_end = pls->pb.buf_ptr = pls->pb.buffer;
        
        pls->pb.pos = 0;
        
        ff_read_frame_flush(pls->ctx);
        pls->seek_timestamp = seek_timestamp;
        pls->seek_flags = flags;
        if (pls != seek_pls) {
            
            find_timestamp_in_playlist(c, pls, seek_timestamp, &pls->cur_seq_no);
            


            pls->seek_stream_index = -1;
            pls->seek_flags |= AVSEEK_FLAG_ANY;
        }
    }
    c->cur_timestamp = seek_timestamp;
    return 0;
}
