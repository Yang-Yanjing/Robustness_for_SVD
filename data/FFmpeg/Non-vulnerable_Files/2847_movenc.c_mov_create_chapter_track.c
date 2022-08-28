
static int mov_create_chapter_track(AVFormatContext *s, int tracknum)
{
    AVIOContext *pb;
    MOVMuxContext *mov = s->priv_data;
    MOVTrack *track = &mov->tracks[tracknum];
    AVPacket pkt = { .stream_index = tracknum, .flags = AV_PKT_FLAG_KEY };
    int i, len;
    track->mode = mov->mode;
    track->tag = MKTAG('t','e','x','t');
    track->timescale = MOV_TIMESCALE;
    track->enc = avcodec_alloc_context3(NULL);
    if (!track->enc)
        return AVERROR(ENOMEM);
    track->enc->codec_type = AVMEDIA_TYPE_SUBTITLE;
#if 0
    
    uint8_t chapter_properties[43] = { 0, 0, 0, 0, 0, 0, 0, 1, };
    if (ff_alloc_extradata(track->enc, sizeof(chapter_properties)))
        return AVERROR(ENOMEM);
    memcpy(track->enc->extradata, chapter_properties, sizeof(chapter_properties));
#else
    if (avio_open_dyn_buf(&pb) >= 0) {
        int size;
        uint8_t *buf;
        
        
        avio_wb32(pb, 0x01); 
        avio_w8(pb, 0x00);   
        avio_w8(pb, 0x00);   
        avio_w8(pb, 0x00);   
        avio_w8(pb, 0x00);   
        avio_w8(pb, 0x00);   
        avio_w8(pb, 0x00);   
        
        avio_wb16(pb, 0x00); 
        avio_wb16(pb, 0x00); 
        avio_wb16(pb, 0x00); 
        avio_wb16(pb, 0x00); 
        
        avio_wb16(pb, 0x00); 
        avio_wb16(pb, 0x00); 
        avio_wb16(pb, 0x01); 
        avio_w8(pb, 0x00);   
        avio_w8(pb, 0x00);   
        avio_w8(pb, 0x00);   
        avio_w8(pb, 0x00);   
        avio_w8(pb, 0x00);   
        avio_w8(pb, 0x00);   
        
        avio_wb32(pb, 0x0D); 
        ffio_wfourcc(pb, "ftab"); 
        avio_wb16(pb, 0x01); 
        
        avio_wb16(pb, 0x01); 
        avio_w8(pb, 0x00);   
        if ((size = avio_close_dyn_buf(pb, &buf)) > 0) {
            track->enc->extradata = buf;
            track->enc->extradata_size = size;
        } else {
            av_freep(&buf);
        }
    }
#endif
    for (i = 0; i < s->nb_chapters; i++) {
        AVChapter *c = s->chapters[i];
        AVDictionaryEntry *t;
        int64_t end = av_rescale_q(c->end, c->time_base, (AVRational){1,MOV_TIMESCALE});
        pkt.pts = pkt.dts = av_rescale_q(c->start, c->time_base, (AVRational){1,MOV_TIMESCALE});
        pkt.duration = end - pkt.dts;
        if ((t = av_dict_get(c->metadata, "title", NULL, 0))) {
            const char encd[12] = {
                0x00, 0x00, 0x00, 0x0C,
                'e',  'n',  'c',  'd',
                0x00, 0x00, 0x01, 0x00 };
            len      = strlen(t->value);
            pkt.size = len + 2 + 12;
            pkt.data = av_malloc(pkt.size);
            if (!pkt.data)
                return AVERROR(ENOMEM);
            AV_WB16(pkt.data, len);
            memcpy(pkt.data + 2, t->value, len);
            memcpy(pkt.data + len + 2, encd, sizeof(encd));
            ff_mov_write_packet(s, &pkt);
            av_freep(&pkt.data);
        }
    }
    return 0;
}
