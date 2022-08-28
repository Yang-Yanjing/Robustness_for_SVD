static int interleave_compare_dts(AVFormatContext *s, AVPacket *next,
                                  AVPacket *pkt)
{
    AVStream *st  = s->streams[pkt->stream_index];
    AVStream *st2 = s->streams[next->stream_index];
    int comp      = av_compare_ts(next->dts, st2->time_base, pkt->dts,
                                  st->time_base);
    if (s->audio_preload && ((st->codec->codec_type == AVMEDIA_TYPE_AUDIO) != (st2->codec->codec_type == AVMEDIA_TYPE_AUDIO))) {
        int64_t ts = av_rescale_q(pkt ->dts, st ->time_base, AV_TIME_BASE_Q) - s->audio_preload*(st ->codec->codec_type == AVMEDIA_TYPE_AUDIO);
        int64_t ts2= av_rescale_q(next->dts, st2->time_base, AV_TIME_BASE_Q) - s->audio_preload*(st2->codec->codec_type == AVMEDIA_TYPE_AUDIO);
        if (ts == ts2) {
            ts= ( pkt ->dts* st->time_base.num*AV_TIME_BASE - s->audio_preload*(int64_t)(st ->codec->codec_type == AVMEDIA_TYPE_AUDIO)* st->time_base.den)*st2->time_base.den
               -( next->dts*st2->time_base.num*AV_TIME_BASE - s->audio_preload*(int64_t)(st2->codec->codec_type == AVMEDIA_TYPE_AUDIO)*st2->time_base.den)* st->time_base.den;
            ts2=0;
        }
        comp= (ts>ts2) - (ts<ts2);
    }
    if (comp == 0)
        return pkt->stream_index < next->stream_index;
    return comp > 0;
}
