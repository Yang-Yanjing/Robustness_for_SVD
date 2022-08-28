int av_write_trailer(AVFormatContext *s)
{
    int ret, i;
    for (;; ) {
        AVPacket pkt;
        ret = interleave_packet(s, &pkt, NULL, 1);
        if (ret < 0)
            goto fail;
        if (!ret)
            break;
        ret = write_packet(s, &pkt);
        if (ret >= 0)
            s->streams[pkt.stream_index]->nb_frames++;
        av_free_packet(&pkt);
        if (ret < 0)
            goto fail;
        if(s->pb && s->pb->error)
            goto fail;
    }
fail:
    if (s->oformat->write_trailer)
        if (ret >= 0) {
        ret = s->oformat->write_trailer(s);
        } else {
            s->oformat->write_trailer(s);
        }
    if (s->pb)
       avio_flush(s->pb);
    if (ret == 0)
       ret = s->pb ? s->pb->error : 0;
    for (i = 0; i < s->nb_streams; i++) {
        av_freep(&s->streams[i]->priv_data);
        av_freep(&s->streams[i]->index_entries);
    }
    if (s->oformat->priv_class)
        av_opt_free(s->priv_data);
    av_freep(&s->priv_data);
    return ret;
}
