static int flv_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    FLVContext *flv = s->priv_data;
    int ret, i, size, flags;
    enum FlvTagType type;
    int stream_type=-1;
    int64_t next, pos, meta_pos;
    int64_t dts, pts = AV_NOPTS_VALUE;
    int av_uninit(channels);
    int av_uninit(sample_rate);
    AVStream *st    = NULL;
    
    for (;; avio_skip(s->pb, 4)) {
        pos  = avio_tell(s->pb);
        type = (avio_r8(s->pb) & 0x1F);
        size = avio_rb24(s->pb);
        dts  = avio_rb24(s->pb);
        dts |= avio_r8(s->pb) << 24;
        av_log(s, AV_LOG_TRACE, "type:%d, size:%d, dts:%"PRId64" pos:%"PRId64"\n", type, size, dts, avio_tell(s->pb));
        if (avio_feof(s->pb))
            return AVERROR_EOF;
        avio_skip(s->pb, 3); 
        flags = 0;
        if (flv->validate_next < flv->validate_count) {
            int64_t validate_pos = flv->validate_index[flv->validate_next].pos;
            if (pos == validate_pos) {
                if (FFABS(dts - flv->validate_index[flv->validate_next].dts) <=
                    VALIDATE_INDEX_TS_THRESH) {
                    flv->validate_next++;
                } else {
                    clear_index_entries(s, validate_pos);
                    flv->validate_count = 0;
                }
            } else if (pos > validate_pos) {
                clear_index_entries(s, validate_pos);
                flv->validate_count = 0;
            }
        }
        if (size == 0)
            continue;
        next = size + avio_tell(s->pb);
        if (type == FLV_TAG_TYPE_AUDIO) {
            stream_type = FLV_STREAM_TYPE_AUDIO;
            flags    = avio_r8(s->pb);
            size--;
        } else if (type == FLV_TAG_TYPE_VIDEO) {
            stream_type = FLV_STREAM_TYPE_VIDEO;
            flags    = avio_r8(s->pb);
            size--;
            if ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_VIDEO_INFO_CMD)
                goto skip;
        } else if (type == FLV_TAG_TYPE_META) {
            stream_type=FLV_STREAM_TYPE_DATA;
            if (size > 13 + 1 + 4) { 
                int type;
                meta_pos = avio_tell(s->pb);
                type = flv_read_metabody(s, next);
                if (type == 0 && dts == 0 || type < 0 || type == TYPE_UNKNOWN) {
                    goto skip;
                } else if (type == TYPE_ONTEXTDATA) {
                    avpriv_request_sample(s, "OnTextData packet");
                    return flv_data_packet(s, pkt, dts, next);
                } else if (type == TYPE_ONCAPTION) {
                    return flv_data_packet(s, pkt, dts, next);
                }
                avio_seek(s->pb, meta_pos, SEEK_SET);
            }
        } else {
            av_log(s, AV_LOG_DEBUG,
                   "Skipping flv packet: type %d, size %d, flags %d.\n",
                   type, size, flags);
skip:
            avio_seek(s->pb, next, SEEK_SET);
            continue;
        }
        
        if (!size)
            continue;
        
        for (i = 0; i < s->nb_streams; i++) {
            st = s->streams[i];
            if (stream_type == FLV_STREAM_TYPE_AUDIO) {
                if (st->codec->codec_type == AVMEDIA_TYPE_AUDIO &&
                    (s->audio_codec_id || flv_same_audio_codec(st->codec, flags)))
                    break;
            } else if (stream_type == FLV_STREAM_TYPE_VIDEO) {
                if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO &&
                    (s->video_codec_id || flv_same_video_codec(st->codec, flags)))
                    break;
            } else if (stream_type == FLV_STREAM_TYPE_DATA) {
                if (st->codec->codec_type == AVMEDIA_TYPE_SUBTITLE)
                    break;
            }
        }
        if (i == s->nb_streams) {
            static const enum AVMediaType stream_types[] = {AVMEDIA_TYPE_VIDEO, AVMEDIA_TYPE_AUDIO, AVMEDIA_TYPE_SUBTITLE};
            av_log(s, AV_LOG_WARNING, "Stream discovered after head already parsed\n");
            st = create_stream(s, stream_types[stream_type]);
            if (!st)
                return AVERROR(ENOMEM);
        }
        av_log(s, AV_LOG_TRACE, "%d %X %d \n", stream_type, flags, st->discard);
        if (s->pb->seekable &&
            ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_KEY ||
              stream_type == FLV_STREAM_TYPE_AUDIO))
            av_add_index_entry(st, pos, dts, size, 0, AVINDEX_KEYFRAME);
        if (  (st->discard >= AVDISCARD_NONKEY && !((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_KEY || (stream_type == FLV_STREAM_TYPE_AUDIO)))
            ||(st->discard >= AVDISCARD_BIDIR  &&  ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_DISP_INTER && (stream_type == FLV_STREAM_TYPE_VIDEO)))
            || st->discard >= AVDISCARD_ALL
        ) {
            avio_seek(s->pb, next, SEEK_SET);
            continue;
        }
        break;
    }
    
    
    if (s->pb->seekable && (!s->duration || s->duration == AV_NOPTS_VALUE) && !flv->searched_for_end) {
        int size;
        const int64_t pos   = avio_tell(s->pb);
        
        
        int64_t fsize       = avio_size(s->pb);
retry_duration:
        avio_seek(s->pb, fsize - 4, SEEK_SET);
        size = avio_rb32(s->pb);
        
        
        avio_seek(s->pb, fsize - 3 - size, SEEK_SET);
        if (size == avio_rb24(s->pb) + 11) {
            uint32_t ts = avio_rb24(s->pb);
            ts         |= avio_r8(s->pb) << 24;
            if (ts)
                s->duration = ts * (int64_t)AV_TIME_BASE / 1000;
            else if (fsize >= 8 && fsize - 8 >= size) {
                fsize -= size+4;
                goto retry_duration;
            }
        }
        avio_seek(s->pb, pos, SEEK_SET);
        flv->searched_for_end = 1;
    }
    if (stream_type == FLV_STREAM_TYPE_AUDIO) {
        int bits_per_coded_sample;
        channels = (flags & FLV_AUDIO_CHANNEL_MASK) == FLV_STEREO ? 2 : 1;
        sample_rate = 44100 << ((flags & FLV_AUDIO_SAMPLERATE_MASK) >>
                                FLV_AUDIO_SAMPLERATE_OFFSET) >> 3;
        bits_per_coded_sample = (flags & FLV_AUDIO_SAMPLESIZE_MASK) ? 16 : 8;
        if (!st->codec->channels || !st->codec->sample_rate ||
            !st->codec->bits_per_coded_sample) {
            st->codec->channels              = channels;
            st->codec->channel_layout        = channels == 1
                                               ? AV_CH_LAYOUT_MONO
                                               : AV_CH_LAYOUT_STEREO;
            st->codec->sample_rate           = sample_rate;
            st->codec->bits_per_coded_sample = bits_per_coded_sample;
        }
        if (!st->codec->codec_id) {
            flv_set_audio_codec(s, st, st->codec,
                                flags & FLV_AUDIO_CODECID_MASK);
            flv->last_sample_rate =
            sample_rate           = st->codec->sample_rate;
            flv->last_channels    =
            channels              = st->codec->channels;
        } else {
            AVCodecContext ctx = {0};
            ctx.sample_rate = sample_rate;
            ctx.bits_per_coded_sample = bits_per_coded_sample;
            flv_set_audio_codec(s, st, &ctx, flags & FLV_AUDIO_CODECID_MASK);
            sample_rate = ctx.sample_rate;
        }
    } else if (stream_type == FLV_STREAM_TYPE_VIDEO) {
        size -= flv_set_video_codec(s, st, flags & FLV_VIDEO_CODECID_MASK, 1);
    } else if (stream_type == FLV_STREAM_TYPE_DATA) {
        st->codec->codec_id = AV_CODEC_ID_TEXT;
    }
    if (st->codec->codec_id == AV_CODEC_ID_AAC ||
        st->codec->codec_id == AV_CODEC_ID_H264 ||
        st->codec->codec_id == AV_CODEC_ID_MPEG4) {
        int type = avio_r8(s->pb);
        size--;
        if (st->codec->codec_id == AV_CODEC_ID_H264 || st->codec->codec_id == AV_CODEC_ID_MPEG4) {
            
            int32_t cts = (avio_rb24(s->pb) + 0xff800000) ^ 0xff800000;
            pts = dts + cts;
            if (cts < 0) { 
                if (!flv->wrong_dts)
                    av_log(s, AV_LOG_WARNING,
                        "Negative cts, previous timestamps might be wrong.\n");
                flv->wrong_dts = 1;
            } else if (FFABS(dts - pts) > 1000*60*15) {
                av_log(s, AV_LOG_WARNING,
                       "invalid timestamps %"PRId64" %"PRId64"\n", dts, pts);
                dts = pts = AV_NOPTS_VALUE;
            }
        }
        if (type == 0 && (!st->codec->extradata || st->codec->codec_id == AV_CODEC_ID_AAC ||
            st->codec->codec_id == AV_CODEC_ID_H264)) {
            AVDictionaryEntry *t;
            if (st->codec->extradata) {
                if ((ret = flv_queue_extradata(flv, s->pb, stream_type, size)) < 0)
                    return ret;
                ret = AVERROR(EAGAIN);
                goto leave;
            }
            if ((ret = flv_get_extradata(s, st, size)) < 0)
                return ret;
            
            t = av_dict_get(s->metadata, "Encoder", NULL, 0);
            if (st->codec->codec_id == AV_CODEC_ID_AAC && t && !strcmp(t->value, "Omnia A/XE"))
                st->codec->extradata_size = 2;
            if (st->codec->codec_id == AV_CODEC_ID_AAC && 0) {
                MPEG4AudioConfig cfg;
                if (avpriv_mpeg4audio_get_config(&cfg, st->codec->extradata,
                                             st->codec->extradata_size * 8, 1) >= 0) {
                st->codec->channels       = cfg.channels;
                st->codec->channel_layout = 0;
                if (cfg.ext_sample_rate)
                    st->codec->sample_rate = cfg.ext_sample_rate;
                else
                    st->codec->sample_rate = cfg.sample_rate;
                av_log(s, AV_LOG_TRACE, "mp4a config channels %d sample rate %d\n",
                        st->codec->channels, st->codec->sample_rate);
                }
            }
            ret = AVERROR(EAGAIN);
            goto leave;
        }
    }
    
    if (!size) {
        ret = AVERROR(EAGAIN);
        goto leave;
    }
    ret = av_get_packet(s->pb, pkt, size);
    if (ret < 0)
        return ret;
    pkt->dts          = dts;
    pkt->pts          = pts == AV_NOPTS_VALUE ? dts : pts;
    pkt->stream_index = st->index;
    if (flv->new_extradata[stream_type]) {
        uint8_t *side = av_packet_new_side_data(pkt, AV_PKT_DATA_NEW_EXTRADATA,
                                                flv->new_extradata_size[stream_type]);
        if (side) {
            memcpy(side, flv->new_extradata[stream_type],
                   flv->new_extradata_size[stream_type]);
            av_freep(&flv->new_extradata[stream_type]);
            flv->new_extradata_size[stream_type] = 0;
        }
    }
    if (stream_type == FLV_STREAM_TYPE_AUDIO &&
                    (sample_rate != flv->last_sample_rate ||
                     channels    != flv->last_channels)) {
        flv->last_sample_rate = sample_rate;
        flv->last_channels    = channels;
        ff_add_param_change(pkt, channels, 0, sample_rate, 0, 0);
    }
    if (    stream_type == FLV_STREAM_TYPE_AUDIO ||
            ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_KEY) ||
            stream_type == FLV_STREAM_TYPE_DATA)
        pkt->flags |= AV_PKT_FLAG_KEY;
leave:
    avio_skip(s->pb, 4);
    return ret;
}
