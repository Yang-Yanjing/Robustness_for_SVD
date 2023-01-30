static int mov_write_hdlr_tag(AVIOContext *pb, MOVTrack *track)
{
    const char *hdlr, *descr = NULL, *hdlr_type = NULL;
    int64_t pos = avio_tell(pb);
    hdlr      = "dhlr";
    hdlr_type = "url ";
    descr     = "DataHandler";
    if (track) {
        hdlr = (track->mode == MODE_MOV) ? "mhlr" : "\0\0\0\0";
        if (track->enc->codec_type == AVMEDIA_TYPE_VIDEO) {
            hdlr_type = "vide";
            descr     = "VideoHandler";
        } else if (track->enc->codec_type == AVMEDIA_TYPE_AUDIO) {
            hdlr_type = "soun";
            descr     = "SoundHandler";
        } else if (track->enc->codec_type == AVMEDIA_TYPE_SUBTITLE) {
            if (is_clcp_track(track)) {
                hdlr_type = "clcp";
                descr = "ClosedCaptionHandler";
            } else {
                if (track->tag == MKTAG('t','x','3','g')) {
                    hdlr_type = "sbtl";
                } else if (track->tag == MKTAG('m','p','4','s')) {
                    hdlr_type = "subp";
                } else {
                    hdlr_type = "text";
                }
            descr = "SubtitleHandler";
            }
        } else if (track->enc->codec_tag == MKTAG('r','t','p',' ')) {
            hdlr_type = "hint";
            descr     = "HintHandler";
        } else if (track->enc->codec_tag == MKTAG('t','m','c','d')) {
            hdlr_type = "tmcd";
            descr = "TimeCodeHandler";
        } else {
            char tag_buf[32];
            av_get_codec_tag_string(tag_buf, sizeof(tag_buf),
                                    track->enc->codec_tag);
            av_log(track->enc, AV_LOG_WARNING,
                   "Unknown hldr_type for %s / 0x%04X, writing dummy values\n",
                   tag_buf, track->enc->codec_tag);
        }
        if (track->st) {
            
            
            
            AVDictionaryEntry *t;
            t = av_dict_get(track->st->metadata, "handler", NULL, 0);
            if (t && utf8len(t->value))
                descr = t->value;
        }
    }
    avio_wb32(pb, 0); 
    ffio_wfourcc(pb, "hdlr");
    avio_wb32(pb, 0); 
    avio_write(pb, hdlr, 4); 
    ffio_wfourcc(pb, hdlr_type); 
    avio_wb32(pb, 0); 
    avio_wb32(pb, 0); 
    avio_wb32(pb, 0); 
    if (!track || track->mode == MODE_MOV)
        avio_w8(pb, strlen(descr)); 
    avio_write(pb, descr, strlen(descr)); 
    if (track && track->mode != MODE_MOV)
        avio_w8(pb, 0); 
    return update_size(pb, pos);
}
