static int mov_create_timecode_track(AVFormatContext *s, int index, int src_index, const char *tcstr)
{
    int ret;
    MOVMuxContext *mov  = s->priv_data;
    MOVTrack *track     = &mov->tracks[index];
    AVStream *src_st    = s->streams[src_index];
    AVTimecode tc;
    AVPacket pkt    = {.stream_index = index, .flags = AV_PKT_FLAG_KEY, .size = 4};
    AVRational rate = find_fps(s, src_st);
    
    ret = av_timecode_init_from_string(&tc, rate, tcstr, s);
    if (ret < 0)
        return ret;
    
    track->mode      = mov->mode;
    track->tag       = MKTAG('t','m','c','d');
    track->src_track = src_index;
    track->timescale = mov->tracks[src_index].timescale;
    if (tc.flags & AV_TIMECODE_FLAG_DROPFRAME)
        track->timecode_flags |= MOV_TIMECODE_FLAG_DROPFRAME;
    
    track->st = src_st;
    
    track->enc = avcodec_alloc_context3(NULL);
    if (!track->enc)
        return AVERROR(ENOMEM);
    track->enc->codec_type = AVMEDIA_TYPE_DATA;
    track->enc->codec_tag  = track->tag;
    track->enc->time_base  = av_inv_q(rate);
    
    pkt.data = av_malloc(pkt.size);
    if (!pkt.data)
        return AVERROR(ENOMEM);
    AV_WB32(pkt.data, tc.start);
    ret = ff_mov_write_packet(s, &pkt);
    av_free(pkt.data);
    return ret;
}
