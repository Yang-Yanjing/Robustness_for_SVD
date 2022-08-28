static int decode_stream_header(NUTContext *nut)
{
    AVFormatContext *s = nut->avf;
    AVIOContext *bc    = s->pb;
    StreamContext *stc;
    int class, stream_id, ret;
    uint64_t tmp, end;
    AVStream *st = NULL;
    end  = get_packetheader(nut, bc, 1, STREAM_STARTCODE);
    end += avio_tell(bc);
    GET_V(stream_id, tmp < s->nb_streams && !nut->stream[tmp].time_base);
    stc = &nut->stream[stream_id];
    st  = s->streams[stream_id];
    if (!st)
        return AVERROR(ENOMEM);
    class                = ffio_read_varlen(bc);
    tmp                  = get_fourcc(bc);
    st->codec->codec_tag = tmp;
    switch (class) {
    case 0:
        st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
        st->codec->codec_id   = av_codec_get_id((const AVCodecTag * const []) {
                                                    ff_nut_video_tags,
                                                    ff_codec_bmp_tags,
                                                    ff_codec_movvideo_tags,
                                                    0
                                                },
                                                tmp);
        break;
    case 1:
        st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
        st->codec->codec_id   = av_codec_get_id((const AVCodecTag * const []) {
                                                    ff_nut_audio_tags,
                                                    ff_codec_wav_tags,
                                                    ff_nut_audio_extra_tags,
                                                    0
                                                },
                                                tmp);
        break;
    case 2:
        st->codec->codec_type = AVMEDIA_TYPE_SUBTITLE;
        st->codec->codec_id   = ff_codec_get_id(ff_nut_subtitle_tags, tmp);
        break;
    case 3:
        st->codec->codec_type = AVMEDIA_TYPE_DATA;
        st->codec->codec_id   = ff_codec_get_id(ff_nut_data_tags, tmp);
        break;
    default:
        av_log(s, AV_LOG_ERROR, "unknown stream class (%d)\n", class);
        return AVERROR(ENOSYS);
    }
    if (class < 3 && st->codec->codec_id == AV_CODEC_ID_NONE)
        av_log(s, AV_LOG_ERROR,
               "Unknown codec tag '0x%04x' for stream number %d\n",
               (unsigned int) tmp, stream_id);
    GET_V(stc->time_base_id, tmp < nut->time_base_count);
    GET_V(stc->msb_pts_shift, tmp < 16);
    stc->max_pts_distance = ffio_read_varlen(bc);
    GET_V(stc->decode_delay, tmp < 1000); 
    st->codec->has_b_frames = stc->decode_delay;
    ffio_read_varlen(bc); 
    GET_V(st->codec->extradata_size, tmp < (1 << 30));
    if (st->codec->extradata_size) {
        if (ff_get_extradata(st->codec, bc, st->codec->extradata_size) < 0)
            return AVERROR(ENOMEM);
    }
    if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        GET_V(st->codec->width,  tmp > 0);
        GET_V(st->codec->height, tmp > 0);
        st->sample_aspect_ratio.num = ffio_read_varlen(bc);
        st->sample_aspect_ratio.den = ffio_read_varlen(bc);
        if ((!st->sample_aspect_ratio.num) != (!st->sample_aspect_ratio.den)) {
            av_log(s, AV_LOG_ERROR, "invalid aspect ratio %d/%d\n",
                   st->sample_aspect_ratio.num, st->sample_aspect_ratio.den);
            ret = AVERROR_INVALIDDATA;
            goto fail;
        }
        ffio_read_varlen(bc); 
    } else if (st->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
        GET_V(st->codec->sample_rate, tmp > 0);
        ffio_read_varlen(bc); 
        GET_V(st->codec->channels, tmp > 0);
    }
    if (skip_reserved(bc, end) || ffio_get_checksum(bc)) {
        av_log(s, AV_LOG_ERROR,
               "stream header %d checksum mismatch\n", stream_id);
        ret = AVERROR_INVALIDDATA;
        goto fail;
    }
    stc->time_base = &nut->time_base[stc->time_base_id];
    avpriv_set_pts_info(s->streams[stream_id], 63, stc->time_base->num,
                        stc->time_base->den);
    return 0;
fail:
    if (st && st->codec) {
        av_freep(&st->codec->extradata);
        st->codec->extradata_size = 0;
    }
    return ret;
}
