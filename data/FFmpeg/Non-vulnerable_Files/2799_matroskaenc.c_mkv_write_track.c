static int mkv_write_track(AVFormatContext *s, MatroskaMuxContext *mkv,
                           int i, AVIOContext *pb, int default_stream_exists)
{
    AVStream *st = s->streams[i];
    AVCodecContext *codec = st->codec;
    ebml_master subinfo, track;
    int native_id = 0;
    int qt_id = 0;
    int bit_depth = av_get_bits_per_sample(codec->codec_id);
    int sample_rate = codec->sample_rate;
    int output_sample_rate = 0;
    int display_width_div = 1;
    int display_height_div = 1;
    int j, ret;
    AVDictionaryEntry *tag;
    
    avpriv_set_pts_info(st, 64, 1, 1000);
    if (codec->codec_type == AVMEDIA_TYPE_ATTACHMENT) {
        mkv->have_attachments = 1;
        return 0;
    }
    if (!bit_depth && codec->codec_id != AV_CODEC_ID_ADPCM_G726)
        bit_depth = av_get_bytes_per_sample(codec->sample_fmt) << 3;
    if (!bit_depth)
        bit_depth = codec->bits_per_coded_sample;
    if (codec->codec_id == AV_CODEC_ID_AAC) {
        ret = get_aac_sample_rates(s, codec, &sample_rate, &output_sample_rate);
        if (ret < 0)
            return ret;
    }
    track = start_ebml_master(pb, MATROSKA_ID_TRACKENTRY, 0);
    put_ebml_uint (pb, MATROSKA_ID_TRACKNUMBER,
                   mkv->is_dash ? mkv->dash_track_number : i + 1);
    put_ebml_uint (pb, MATROSKA_ID_TRACKUID,
                   mkv->is_dash ? mkv->dash_track_number : i + 1);
    put_ebml_uint (pb, MATROSKA_ID_TRACKFLAGLACING , 0);    
    if ((tag = av_dict_get(st->metadata, "title", NULL, 0)))
        put_ebml_string(pb, MATROSKA_ID_TRACKNAME, tag->value);
    tag = av_dict_get(st->metadata, "language", NULL, 0);
    if (mkv->mode != MODE_WEBM || codec->codec_id != AV_CODEC_ID_WEBVTT) {
        put_ebml_string(pb, MATROSKA_ID_TRACKLANGUAGE, tag && tag->value ? tag->value:"und");
    } else if (tag && tag->value) {
        put_ebml_string(pb, MATROSKA_ID_TRACKLANGUAGE, tag->value);
    }
    
    
    if (default_stream_exists && !(st->disposition & AV_DISPOSITION_DEFAULT))
        put_ebml_uint(pb, MATROSKA_ID_TRACKFLAGDEFAULT, !!(st->disposition & AV_DISPOSITION_DEFAULT));
    if (st->disposition & AV_DISPOSITION_FORCED)
        put_ebml_uint(pb, MATROSKA_ID_TRACKFLAGFORCED, 1);
    if (mkv->mode == MODE_WEBM && codec->codec_id == AV_CODEC_ID_WEBVTT) {
        const char *codec_id;
        if (st->disposition & AV_DISPOSITION_CAPTIONS) {
            codec_id = "D_WEBVTT/CAPTIONS";
            native_id = MATROSKA_TRACK_TYPE_SUBTITLE;
        } else if (st->disposition & AV_DISPOSITION_DESCRIPTIONS) {
            codec_id = "D_WEBVTT/DESCRIPTIONS";
            native_id = MATROSKA_TRACK_TYPE_METADATA;
        } else if (st->disposition & AV_DISPOSITION_METADATA) {
            codec_id = "D_WEBVTT/METADATA";
            native_id = MATROSKA_TRACK_TYPE_METADATA;
        } else {
            codec_id = "D_WEBVTT/SUBTITLES";
            native_id = MATROSKA_TRACK_TYPE_SUBTITLE;
        }
        put_ebml_string(pb, MATROSKA_ID_CODECID, codec_id);
    } else {
        
        
        for (j = 0; ff_mkv_codec_tags[j].id != AV_CODEC_ID_NONE; j++) {
            if (ff_mkv_codec_tags[j].id == codec->codec_id) {
                put_ebml_string(pb, MATROSKA_ID_CODECID, ff_mkv_codec_tags[j].str);
                native_id = 1;
                break;
            }
        }
        if (codec->codec_id == AV_CODEC_ID_RAWVIDEO && !codec->codec_tag) {
            if (mkv->allow_raw_vfw) {
                native_id = 0;
            } else {
                av_log(s, AV_LOG_ERROR, "Raw RGB is not supported Natively in Matroska, you can use AVI or NUT or\n"
                                        "If you would like to store it anyway using VFW mode, enable allow_raw_vfw (-allow_raw_vfw 1)\n");
                return AVERROR(EINVAL);
            }
        }
    }
    if (codec->codec_type == AVMEDIA_TYPE_AUDIO && codec->initial_padding && codec->codec_id == AV_CODEC_ID_OPUS) {



        put_ebml_uint(pb, MATROSKA_ID_CODECDELAY,
                      av_rescale_q(codec->initial_padding,
                                   (AVRational){ 1, codec->sample_rate },
                                   (AVRational){ 1, 1000000000 }));
    }
    if (codec->codec_id == AV_CODEC_ID_OPUS) {
        put_ebml_uint(pb, MATROSKA_ID_SEEKPREROLL, OPUS_SEEK_PREROLL);
    }
    if (mkv->mode == MODE_WEBM && !(codec->codec_id == AV_CODEC_ID_VP8 ||
                                    codec->codec_id == AV_CODEC_ID_VP9 ||
                                    codec->codec_id == AV_CODEC_ID_OPUS ||
                                    codec->codec_id == AV_CODEC_ID_VORBIS ||
                                    codec->codec_id == AV_CODEC_ID_WEBVTT)) {
        av_log(s, AV_LOG_ERROR,
               "Only VP8 or VP9 video and Vorbis or Opus audio and WebVTT subtitles are supported for WebM.\n");
        return AVERROR(EINVAL);
    }
    switch (codec->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        put_ebml_uint(pb, MATROSKA_ID_TRACKTYPE, MATROSKA_TRACK_TYPE_VIDEO);
        if(   st->avg_frame_rate.num > 0 && st->avg_frame_rate.den > 0
           && av_cmp_q(av_inv_q(st->avg_frame_rate), codec->time_base) > 0)
            put_ebml_uint(pb, MATROSKA_ID_TRACKDEFAULTDURATION, 1000000000LL * st->avg_frame_rate.den / st->avg_frame_rate.num);
        else
            put_ebml_uint(pb, MATROSKA_ID_TRACKDEFAULTDURATION, 1000000000LL * codec->time_base.num / codec->time_base.den);
        if (!native_id &&
            ff_codec_get_tag(ff_codec_movvideo_tags, codec->codec_id) &&
            ((!ff_codec_get_tag(ff_codec_bmp_tags,   codec->codec_id) && codec->codec_id != AV_CODEC_ID_RAWVIDEO) ||
             codec->codec_id == AV_CODEC_ID_SVQ1 ||
             codec->codec_id == AV_CODEC_ID_SVQ3 ||
             codec->codec_id == AV_CODEC_ID_CINEPAK))
            qt_id = 1;
        if (qt_id)
            put_ebml_string(pb, MATROSKA_ID_CODECID, "V_QUICKTIME");
        else if (!native_id) {
            
            put_ebml_string(pb, MATROSKA_ID_CODECID, "V_MS/VFW/FOURCC");
            mkv->tracks[i].write_dts = 1;
            s->internal->avoid_negative_ts_use_pts = 0;
        }
        subinfo = start_ebml_master(pb, MATROSKA_ID_TRACKVIDEO, 0);
        
        put_ebml_uint (pb, MATROSKA_ID_VIDEOPIXELWIDTH , codec->width);
        put_ebml_uint (pb, MATROSKA_ID_VIDEOPIXELHEIGHT, codec->height);
        
        
        ret = mkv_write_stereo_mode(s, pb, st, mkv->mode,
                                    &display_width_div,
                                    &display_height_div);
        if (ret < 0)
            return ret;
        if (((tag = av_dict_get(st->metadata, "alpha_mode", NULL, 0)) && atoi(tag->value)) ||
            ((tag = av_dict_get( s->metadata, "alpha_mode", NULL, 0)) && atoi(tag->value)) ||
            (codec->pix_fmt == AV_PIX_FMT_YUVA420P)) {
            put_ebml_uint(pb, MATROSKA_ID_VIDEOALPHAMODE, 1);
        }
        
        
        if (st->sample_aspect_ratio.num) {
            int64_t d_width = av_rescale(codec->width, st->sample_aspect_ratio.num, st->sample_aspect_ratio.den);
            if (d_width > INT_MAX) {
                av_log(s, AV_LOG_ERROR, "Overflow in display width\n");
                return AVERROR(EINVAL);
            }
            put_ebml_uint(pb, MATROSKA_ID_VIDEODISPLAYWIDTH , d_width / display_width_div);
            put_ebml_uint(pb, MATROSKA_ID_VIDEODISPLAYHEIGHT, codec->height / display_height_div);
        } else if (display_width_div != 1 || display_height_div != 1) {
            put_ebml_uint(pb, MATROSKA_ID_VIDEODISPLAYWIDTH , codec->width / display_width_div);
            put_ebml_uint(pb, MATROSKA_ID_VIDEODISPLAYHEIGHT, codec->height / display_height_div);
        }
        if (codec->codec_id == AV_CODEC_ID_RAWVIDEO) {
            uint32_t color_space = av_le2ne32(codec->codec_tag);
            put_ebml_binary(pb, MATROSKA_ID_VIDEOCOLORSPACE, &color_space, sizeof(color_space));
        }
        end_ebml_master(pb, subinfo);
        break;
    case AVMEDIA_TYPE_AUDIO:
        put_ebml_uint(pb, MATROSKA_ID_TRACKTYPE, MATROSKA_TRACK_TYPE_AUDIO);
        if (!native_id)
            
            put_ebml_string(pb, MATROSKA_ID_CODECID, "A_MS/ACM");
        subinfo = start_ebml_master(pb, MATROSKA_ID_TRACKAUDIO, 0);
        put_ebml_uint  (pb, MATROSKA_ID_AUDIOCHANNELS    , codec->channels);
        put_ebml_float (pb, MATROSKA_ID_AUDIOSAMPLINGFREQ, sample_rate);
        if (output_sample_rate)
            put_ebml_float(pb, MATROSKA_ID_AUDIOOUTSAMPLINGFREQ, output_sample_rate);
        if (bit_depth)
            put_ebml_uint(pb, MATROSKA_ID_AUDIOBITDEPTH, bit_depth);
        end_ebml_master(pb, subinfo);
        break;
    case AVMEDIA_TYPE_SUBTITLE:
        if (!native_id) {
            av_log(s, AV_LOG_ERROR, "Subtitle codec %d is not supported.\n", codec->codec_id);
            return AVERROR(ENOSYS);
        }
        if (mkv->mode != MODE_WEBM || codec->codec_id != AV_CODEC_ID_WEBVTT)
            native_id = MATROSKA_TRACK_TYPE_SUBTITLE;
        put_ebml_uint(pb, MATROSKA_ID_TRACKTYPE, native_id);
        break;
    default:
        av_log(s, AV_LOG_ERROR, "Only audio, video, and subtitles are supported for Matroska.\n");
        return AVERROR(EINVAL);
    }
    if (mkv->mode != MODE_WEBM || codec->codec_id != AV_CODEC_ID_WEBVTT) {
        ret = mkv_write_codecprivate(s, pb, codec, native_id, qt_id);
        if (ret < 0)
            return ret;
    }
    end_ebml_master(pb, track);
    return 0;
}
