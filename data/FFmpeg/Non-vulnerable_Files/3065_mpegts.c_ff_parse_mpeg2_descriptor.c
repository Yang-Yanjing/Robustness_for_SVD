int ff_parse_mpeg2_descriptor(AVFormatContext *fc, AVStream *st, int stream_type,
                              const uint8_t **pp, const uint8_t *desc_list_end,
                              Mp4Descr *mp4_descr, int mp4_descr_count, int pid,
                              MpegTSContext *ts)
{
    const uint8_t *desc_end;
    int desc_len, desc_tag, desc_es_id, ext_desc_tag, channels, channel_config_code;
    char language[252];
    int i;
    desc_tag = get8(pp, desc_list_end);
    if (desc_tag < 0)
        return AVERROR_INVALIDDATA;
    desc_len = get8(pp, desc_list_end);
    if (desc_len < 0)
        return AVERROR_INVALIDDATA;
    desc_end = *pp + desc_len;
    if (desc_end > desc_list_end)
        return AVERROR_INVALIDDATA;
    av_log(fc, AV_LOG_TRACE, "tag: 0x%02x len=%d\n", desc_tag, desc_len);
    if ((st->codec->codec_id == AV_CODEC_ID_NONE || st->request_probe > 0) &&
        stream_type == STREAM_TYPE_PRIVATE_DATA)
        mpegts_find_stream_type(st, desc_tag, DESC_types);
    switch (desc_tag) {
    case 0x1E: 
        desc_es_id = get16(pp, desc_end);
        if (desc_es_id < 0)
            break;
        if (ts && ts->pids[pid])
            ts->pids[pid]->es_id = desc_es_id;
        for (i = 0; i < mp4_descr_count; i++)
            if (mp4_descr[i].dec_config_descr_len &&
                mp4_descr[i].es_id == desc_es_id) {
                AVIOContext pb;
                ffio_init_context(&pb, mp4_descr[i].dec_config_descr,
                                  mp4_descr[i].dec_config_descr_len, 0,
                                  NULL, NULL, NULL, NULL);
                ff_mp4_read_dec_config_descr(fc, st, &pb);
                if (st->codec->codec_id == AV_CODEC_ID_AAC &&
                    st->codec->extradata_size > 0)
                    st->need_parsing = 0;
                if (st->codec->codec_id == AV_CODEC_ID_MPEG4SYSTEMS)
                    mpegts_open_section_filter(ts, pid, m4sl_cb, ts, 1);
            }
        break;
    case 0x1F: 
        if (get16(pp, desc_end) < 0)
            break;
        if (mp4_descr_count > 0 &&
            (st->codec->codec_id == AV_CODEC_ID_AAC_LATM ||
             (st->request_probe == 0 && st->codec->codec_id == AV_CODEC_ID_NONE) ||
             st->request_probe > 0) &&
            mp4_descr->dec_config_descr_len && mp4_descr->es_id == pid) {
            AVIOContext pb;
            ffio_init_context(&pb, mp4_descr->dec_config_descr,
                              mp4_descr->dec_config_descr_len, 0,
                              NULL, NULL, NULL, NULL);
            ff_mp4_read_dec_config_descr(fc, st, &pb);
            if (st->codec->codec_id == AV_CODEC_ID_AAC &&
                st->codec->extradata_size > 0) {
                st->request_probe = st->need_parsing = 0;
                st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
            }
        }
        break;
    case 0x56: 
        {
            uint8_t *extradata = NULL;
            int language_count = desc_len / 5;
            if (desc_len > 0 && desc_len % 5 != 0)
                return AVERROR_INVALIDDATA;
            if (language_count > 0) {
                
                av_assert0(language_count <= sizeof(language) / 4);
                if (st->codec->extradata == NULL) {
                    if (ff_alloc_extradata(st->codec, language_count * 2)) {
                        return AVERROR(ENOMEM);
                    }
                }
               if (st->codec->extradata_size < language_count * 2)
                   return AVERROR_INVALIDDATA;
               extradata = st->codec->extradata;
                for (i = 0; i < language_count; i++) {
                    language[i * 4 + 0] = get8(pp, desc_end);
                    language[i * 4 + 1] = get8(pp, desc_end);
                    language[i * 4 + 2] = get8(pp, desc_end);
                    language[i * 4 + 3] = ',';
                    memcpy(extradata, *pp, 2);
                    extradata += 2;
                    *pp += 2;
                }
                language[i * 4 - 1] = 0;
                av_dict_set(&st->metadata, "language", language, 0);
            }
        }
        break;
    case 0x59: 
        {
            




            int language_count = desc_len / 8;
            if (desc_len > 0 && desc_len % 8 != 0)
                return AVERROR_INVALIDDATA;
            if (language_count > 1) {
                avpriv_request_sample(fc, "DVB subtitles with multiple languages");
            }
            if (language_count > 0) {
                uint8_t *extradata;
                
                av_assert0(language_count <= sizeof(language) / 4);
                if (st->codec->extradata == NULL) {
                    if (ff_alloc_extradata(st->codec, language_count * 5)) {
                        return AVERROR(ENOMEM);
                    }
                }
                if (st->codec->extradata_size < language_count * 5)
                    return AVERROR_INVALIDDATA;
                extradata = st->codec->extradata;
                for (i = 0; i < language_count; i++) {
                    language[i * 4 + 0] = get8(pp, desc_end);
                    language[i * 4 + 1] = get8(pp, desc_end);
                    language[i * 4 + 2] = get8(pp, desc_end);
                    language[i * 4 + 3] = ',';
                    
                    switch (*pp[0]) {
                    case 0x20: 
                    case 0x21: 
                    case 0x22: 
                    case 0x23: 
                    case 0x24: 
                    case 0x25: 
                        st->disposition |= AV_DISPOSITION_HEARING_IMPAIRED;
                        break;
                    }
                    extradata[4] = get8(pp, desc_end); 
                    memcpy(extradata, *pp, 4); 
                    extradata += 5;
                    *pp += 4;
                }
                language[i * 4 - 1] = 0;
                av_dict_set(&st->metadata, "language", language, 0);
            }
        }
        break;
    case 0x0a: 
        for (i = 0; i + 4 <= desc_len; i += 4) {
            language[i + 0] = get8(pp, desc_end);
            language[i + 1] = get8(pp, desc_end);
            language[i + 2] = get8(pp, desc_end);
            language[i + 3] = ',';
            switch (get8(pp, desc_end)) {
            case 0x01:
                st->disposition |= AV_DISPOSITION_CLEAN_EFFECTS;
                break;
            case 0x02:
                st->disposition |= AV_DISPOSITION_HEARING_IMPAIRED;
                break;
            case 0x03:
                st->disposition |= AV_DISPOSITION_VISUAL_IMPAIRED;
                break;
            }
        }
        if (i && language[0]) {
            language[i - 1] = 0;
            av_dict_set(&st->metadata, "language", language, 0);
        }
        break;
    case 0x05: 
        st->codec->codec_tag = bytestream_get_le32(pp);
        av_log(fc, AV_LOG_TRACE, "reg_desc=%.4s\n", (char *)&st->codec->codec_tag);
        if (st->codec->codec_id == AV_CODEC_ID_NONE || st->request_probe > 0)
            mpegts_find_stream_type(st, st->codec->codec_tag, REGD_types);
        break;
    case 0x52: 
        st->stream_identifier = 1 + get8(pp, desc_end);
        break;
    case 0x26: 
        if (get16(pp, desc_end) == 0xFFFF)
            *pp += 4;
        if (get8(pp, desc_end) == 0xFF) {
            st->codec->codec_tag = bytestream_get_le32(pp);
            if (st->codec->codec_id == AV_CODEC_ID_NONE)
                mpegts_find_stream_type(st, st->codec->codec_tag, METADATA_types);
        }
        break;
    case 0x7f: 
        ext_desc_tag = get8(pp, desc_end);
        if (ext_desc_tag < 0)
            return AVERROR_INVALIDDATA;
        if (st->codec->codec_id == AV_CODEC_ID_OPUS &&
            ext_desc_tag == 0x80) { 
            if (!st->codec->extradata) {
                st->codec->extradata = av_mallocz(sizeof(opus_default_extradata) +
                                                  AV_INPUT_BUFFER_PADDING_SIZE);
                if (!st->codec->extradata)
                    return AVERROR(ENOMEM);
                st->codec->extradata_size = sizeof(opus_default_extradata);
                memcpy(st->codec->extradata, opus_default_extradata, sizeof(opus_default_extradata));
                channel_config_code = get8(pp, desc_end);
                if (channel_config_code < 0)
                    return AVERROR_INVALIDDATA;
                if (channel_config_code <= 0x8) {
                    st->codec->extradata[9]  = channels = channel_config_code ? channel_config_code : 2;
                    st->codec->extradata[18] = channel_config_code ? (channels > 2) :  255;
                    st->codec->extradata[19] = opus_stream_cnt[channel_config_code];
                    st->codec->extradata[20] = opus_coupled_stream_cnt[channel_config_code];
                    memcpy(&st->codec->extradata[21], opus_channel_map[channels - 1], channels);
                } else {
                    avpriv_request_sample(fc, "Opus in MPEG-TS - channel_config_code > 0x8");
                }
                st->need_parsing = AVSTREAM_PARSE_FULL;
            }
        }
        break;
    default:
        break;
    }
    *pp = desc_end;
    return 0;
}
