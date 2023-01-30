static int mkv_write_header(AVFormatContext *s)
{
    MatroskaMuxContext *mkv = s->priv_data;
    AVIOContext *pb = s->pb;
    ebml_master ebml_header, segment_info;
    AVDictionaryEntry *tag;
    int ret, i, version = 2;
    if (!strcmp(s->oformat->name, "webm"))
        mkv->mode = MODE_WEBM;
    else
        mkv->mode = MODE_MATROSKAv2;
    if (s->avoid_negative_ts < 0) {
        s->avoid_negative_ts = 1;
        s->internal->avoid_negative_ts_use_pts = 1;
    }
    if (mkv->mode != MODE_WEBM ||
        av_dict_get(s->metadata, "stereo_mode", NULL, 0) ||
        av_dict_get(s->metadata, "alpha_mode", NULL, 0))
        version = 4;
    for (i = 0; i < s->nb_streams; i++) {
        if (s->streams[i]->codec->codec_id == AV_CODEC_ID_ATRAC3 ||
            s->streams[i]->codec->codec_id == AV_CODEC_ID_COOK ||
            s->streams[i]->codec->codec_id == AV_CODEC_ID_RA_288 ||
            s->streams[i]->codec->codec_id == AV_CODEC_ID_SIPR ||
            s->streams[i]->codec->codec_id == AV_CODEC_ID_RV10 ||
            s->streams[i]->codec->codec_id == AV_CODEC_ID_RV20) {
            av_log(s, AV_LOG_ERROR,
                   "The Matroska muxer does not yet support muxing %s\n",
                   avcodec_get_name(s->streams[i]->codec->codec_id));
            return AVERROR_PATCHWELCOME;
        }
        if (s->streams[i]->codec->codec_id == AV_CODEC_ID_OPUS ||
            av_dict_get(s->streams[i]->metadata, "stereo_mode", NULL, 0) ||
            av_dict_get(s->streams[i]->metadata, "alpha_mode", NULL, 0))
            version = 4;
    }
    mkv->tracks = av_mallocz_array(s->nb_streams, sizeof(*mkv->tracks));
    if (!mkv->tracks) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    ebml_header = start_ebml_master(pb, EBML_ID_HEADER, 0);
    put_ebml_uint   (pb, EBML_ID_EBMLVERSION        ,           1);
    put_ebml_uint   (pb, EBML_ID_EBMLREADVERSION    ,           1);
    put_ebml_uint   (pb, EBML_ID_EBMLMAXIDLENGTH    ,           4);
    put_ebml_uint   (pb, EBML_ID_EBMLMAXSIZELENGTH  ,           8);
    put_ebml_string (pb, EBML_ID_DOCTYPE            , s->oformat->name);
    put_ebml_uint   (pb, EBML_ID_DOCTYPEVERSION     ,     version);
    put_ebml_uint   (pb, EBML_ID_DOCTYPEREADVERSION ,           2);
    end_ebml_master(pb, ebml_header);
    mkv->segment = start_ebml_master(pb, MATROSKA_ID_SEGMENT, 0);
    mkv->segment_offset = avio_tell(pb);
    
    
    
    
    
    mkv->main_seekhead    = mkv_start_seekhead(pb, mkv->segment_offset, 10);
    if (!mkv->main_seekhead) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    ret = mkv_add_seekhead_entry(mkv->main_seekhead, MATROSKA_ID_INFO, avio_tell(pb));
    if (ret < 0) goto fail;
    segment_info = start_ebml_master(pb, MATROSKA_ID_INFO, 0);
    put_ebml_uint(pb, MATROSKA_ID_TIMECODESCALE, 1000000);
    if ((tag = av_dict_get(s->metadata, "title", NULL, 0)))
        put_ebml_string(pb, MATROSKA_ID_TITLE, tag->value);
    if (!(s->flags & AVFMT_FLAG_BITEXACT)) {
        uint32_t segment_uid[4];
        AVLFG lfg;
        av_lfg_init(&lfg, av_get_random_seed());
        for (i = 0; i < 4; i++)
            segment_uid[i] = av_lfg_get(&lfg);
        put_ebml_string(pb, MATROSKA_ID_MUXINGAPP, LIBAVFORMAT_IDENT);
        if ((tag = av_dict_get(s->metadata, "encoding_tool", NULL, 0)))
            put_ebml_string(pb, MATROSKA_ID_WRITINGAPP, tag->value);
        else
            put_ebml_string(pb, MATROSKA_ID_WRITINGAPP, LIBAVFORMAT_IDENT);
        put_ebml_binary(pb, MATROSKA_ID_SEGMENTUID, segment_uid, 16);
    } else {
        const char *ident = "Lavf";
        put_ebml_string(pb, MATROSKA_ID_MUXINGAPP , ident);
        put_ebml_string(pb, MATROSKA_ID_WRITINGAPP, ident);
    }
    if (tag = av_dict_get(s->metadata, "creation_time", NULL, 0)) {
        
        int64_t date_utc = (ff_iso8601_to_unix_time(tag->value) - 978307200) * 1000000000;
        uint8_t date_utc_buf[8];
        AV_WB64(date_utc_buf, date_utc);
        put_ebml_binary(pb, MATROSKA_ID_DATEUTC, date_utc_buf, 8);
    }
    
    mkv->duration = 0;
    mkv->duration_offset = avio_tell(pb);
    if (!mkv->is_live) {
        put_ebml_void(pb, 11);              
    }
    end_ebml_master(pb, segment_info);
    
    mkv->stream_durations = av_mallocz(s->nb_streams * sizeof(int64_t));
    mkv->stream_duration_offsets = av_mallocz(s->nb_streams * sizeof(int64_t));
    ret = mkv_write_tracks(s);
    if (ret < 0)
        goto fail;
    for (i = 0; i < s->nb_chapters; i++)
        mkv->chapter_id_offset = FFMAX(mkv->chapter_id_offset, 1LL - s->chapters[i]->id);
    if (mkv->mode != MODE_WEBM) {
        ret = mkv_write_chapters(s);
        if (ret < 0)
            goto fail;
        ret = mkv_write_tags(s);
        if (ret < 0)
            goto fail;
        ret = mkv_write_attachments(s);
        if (ret < 0)
            goto fail;
    }
    if (!s->pb->seekable && !mkv->is_live)
        mkv_write_seekhead(pb, mkv);
    mkv->cues = mkv_start_cues(mkv->segment_offset);
    if (!mkv->cues) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    if (pb->seekable && mkv->reserve_cues_space) {
        mkv->cues_pos = avio_tell(pb);
        put_ebml_void(pb, mkv->reserve_cues_space);
    }
    av_init_packet(&mkv->cur_audio_pkt);
    mkv->cur_audio_pkt.size = 0;
    mkv->cluster_pos = -1;
    avio_flush(pb);
    
    
    if (pb->seekable) {
        if (mkv->cluster_time_limit < 0)
            mkv->cluster_time_limit = 5000;
        if (mkv->cluster_size_limit < 0)
            mkv->cluster_size_limit = 5 * 1024 * 1024;
    } else {
        if (mkv->cluster_time_limit < 0)
            mkv->cluster_time_limit = 1000;
        if (mkv->cluster_size_limit < 0)
            mkv->cluster_size_limit = 32 * 1024;
    }
    return 0;
fail:
    mkv_free(mkv);
    return ret;
}
