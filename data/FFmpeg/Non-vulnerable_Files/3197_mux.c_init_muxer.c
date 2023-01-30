static int init_muxer(AVFormatContext *s, AVDictionary **options)
{
    int ret = 0, i;
    AVStream *st;
    AVDictionary *tmp = NULL;
    AVCodecContext *codec = NULL;
    AVOutputFormat *of = s->oformat;
    AVDictionaryEntry *e;
    if (options)
        av_dict_copy(&tmp, *options, 0);
    if ((ret = av_opt_set_dict(s, &tmp)) < 0)
        goto fail;
    if (s->priv_data && s->oformat->priv_class && *(const AVClass**)s->priv_data==s->oformat->priv_class &&
        (ret = av_opt_set_dict2(s->priv_data, &tmp, AV_OPT_SEARCH_CHILDREN)) < 0)
        goto fail;
    if (s->nb_streams && s->streams[0]->codec->flags & AV_CODEC_FLAG_BITEXACT) {
        if (!(s->flags & AVFMT_FLAG_BITEXACT)) {
#if FF_API_LAVF_BITEXACT
            av_log(s, AV_LOG_WARNING,
                   "Setting the AVFormatContext to bitexact mode, because "
                   "the AVCodecContext is in that mode. This behavior will "
                   "change in the future. To keep the current behavior, set "
                   "AVFormatContext.flags |= AVFMT_FLAG_BITEXACT.\n");
            s->flags |= AVFMT_FLAG_BITEXACT;
#else
            av_log(s, AV_LOG_WARNING,
                   "The AVFormatContext is not in set to bitexact mode, only "
                   "the AVCodecContext. If this is not intended, set "
                   "AVFormatContext.flags |= AVFMT_FLAG_BITEXACT.\n");
#endif
        }
    }
    
    if (s->nb_streams == 0 && !(of->flags & AVFMT_NOSTREAMS)) {
        av_log(s, AV_LOG_ERROR, "No streams to mux were specified\n");
        ret = AVERROR(EINVAL);
        goto fail;
    }
    for (i = 0; i < s->nb_streams; i++) {
        st    = s->streams[i];
        codec = st->codec;
#if FF_API_LAVF_CODEC_TB
FF_DISABLE_DEPRECATION_WARNINGS
        if (!st->time_base.num && codec->time_base.num) {
            av_log(s, AV_LOG_WARNING, "Using AVStream.codec.time_base as a "
                   "timebase hint to the muxer is deprecated. Set "
                   "AVStream.time_base instead.\n");
            avpriv_set_pts_info(st, 64, codec->time_base.num, codec->time_base.den);
        }
FF_ENABLE_DEPRECATION_WARNINGS
#endif
        if (!st->time_base.num) {
            
            if (codec->codec_type == AVMEDIA_TYPE_AUDIO && codec->sample_rate)
                avpriv_set_pts_info(st, 64, 1, codec->sample_rate);
            else
                avpriv_set_pts_info(st, 33, 1, 90000);
        }
        switch (codec->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            if (codec->sample_rate <= 0) {
                av_log(s, AV_LOG_ERROR, "sample rate not set\n");
                ret = AVERROR(EINVAL);
                goto fail;
            }
            if (!codec->block_align)
                codec->block_align = codec->channels *
                                     av_get_bits_per_sample(codec->codec_id) >> 3;
            break;
        case AVMEDIA_TYPE_VIDEO:
            if ((codec->width <= 0 || codec->height <= 0) &&
                !(of->flags & AVFMT_NODIMENSIONS)) {
                av_log(s, AV_LOG_ERROR, "dimensions not set\n");
                ret = AVERROR(EINVAL);
                goto fail;
            }
            if (av_cmp_q(st->sample_aspect_ratio, codec->sample_aspect_ratio)
                && FFABS(av_q2d(st->sample_aspect_ratio) - av_q2d(codec->sample_aspect_ratio)) > 0.004*av_q2d(st->sample_aspect_ratio)
            ) {
                if (st->sample_aspect_ratio.num != 0 &&
                    st->sample_aspect_ratio.den != 0 &&
                    codec->sample_aspect_ratio.num != 0 &&
                    codec->sample_aspect_ratio.den != 0) {
                    av_log(s, AV_LOG_ERROR, "Aspect ratio mismatch between muxer "
                           "(%d/%d) and encoder layer (%d/%d)\n",
                           st->sample_aspect_ratio.num, st->sample_aspect_ratio.den,
                           codec->sample_aspect_ratio.num,
                           codec->sample_aspect_ratio.den);
                    ret = AVERROR(EINVAL);
                    goto fail;
                }
            }
            break;
        }
        if (of->codec_tag) {
            if (   codec->codec_tag
                && codec->codec_id == AV_CODEC_ID_RAWVIDEO
                && (   av_codec_get_tag(of->codec_tag, codec->codec_id) == 0
                    || av_codec_get_tag(of->codec_tag, codec->codec_id) == MKTAG('r', 'a', 'w', ' '))
                && !validate_codec_tag(s, st)) {
                
                
                codec->codec_tag = 0;
            }
            if (codec->codec_tag) {
                if (!validate_codec_tag(s, st)) {
                    char tagbuf[32], tagbuf2[32];
                    av_get_codec_tag_string(tagbuf, sizeof(tagbuf), codec->codec_tag);
                    av_get_codec_tag_string(tagbuf2, sizeof(tagbuf2), av_codec_get_tag(s->oformat->codec_tag, codec->codec_id));
                    av_log(s, AV_LOG_ERROR,
                           "Tag %s/0x%08x incompatible with output codec id '%d' (%s)\n",
                           tagbuf, codec->codec_tag, codec->codec_id, tagbuf2);
                    ret = AVERROR_INVALIDDATA;
                    goto fail;
                }
            } else
                codec->codec_tag = av_codec_get_tag(of->codec_tag, codec->codec_id);
        }
        if (of->flags & AVFMT_GLOBALHEADER &&
            !(codec->flags & AV_CODEC_FLAG_GLOBAL_HEADER))
            av_log(s, AV_LOG_WARNING,
                   "Codec for stream %d does not use global headers "
                   "but container format requires global headers\n", i);
        if (codec->codec_type != AVMEDIA_TYPE_ATTACHMENT)
            s->internal->nb_interleaved_streams++;
    }
    if (!s->priv_data && of->priv_data_size > 0) {
        s->priv_data = av_mallocz(of->priv_data_size);
        if (!s->priv_data) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        if (of->priv_class) {
            *(const AVClass **)s->priv_data = of->priv_class;
            av_opt_set_defaults(s->priv_data);
            if ((ret = av_opt_set_dict2(s->priv_data, &tmp, AV_OPT_SEARCH_CHILDREN)) < 0)
                goto fail;
        }
    }
    
    if (!(s->flags & AVFMT_FLAG_BITEXACT)) {
        av_dict_set(&s->metadata, "encoder", LIBAVFORMAT_IDENT, 0);
    } else {
        av_dict_set(&s->metadata, "encoder", NULL, 0);
    }
    for (e = NULL; e = av_dict_get(s->metadata, "encoder-", e, AV_DICT_IGNORE_SUFFIX); ) {
        av_dict_set(&s->metadata, e->key, NULL, 0);
    }
    if (options) {
         av_dict_free(options);
         *options = tmp;
    }
    return 0;
fail:
    av_dict_free(&tmp);
    return ret;
}
