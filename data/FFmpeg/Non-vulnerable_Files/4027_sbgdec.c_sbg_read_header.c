static av_cold int sbg_read_header(AVFormatContext *avf)
{
    struct sbg_demuxer *sbg = avf->priv_data;
    int r;
    char *buf = NULL;
    struct sbg_script script = { 0 };
    AVStream *st;
    struct ws_intervals inter = { 0 };
    r = read_whole_file(avf->pb, sbg->max_file_size, &buf);
    if (r < 0)
        goto fail;
    r = parse_script(avf, buf, r, &script);
    if (r < 0)
        goto fail;
    if (!sbg->sample_rate)
        sbg->sample_rate = script.sample_rate;
    else
        script.sample_rate = sbg->sample_rate;
    if (!sbg->frame_size)
        sbg->frame_size = FFMAX(1, sbg->sample_rate / 10);
    if (script.opt_mix)
        av_log(avf, AV_LOG_WARNING, "Mix feature not implemented: "
               "-m is ignored and mix channels will be silent.\n");
    r = expand_script(avf, &script);
    if (r < 0)
        goto fail;
    av_freep(&buf);
    r = generate_intervals(avf, &script, sbg->sample_rate, &inter);
    if (r < 0)
        goto fail;
    st = avformat_new_stream(avf, NULL);
    if (!st)
        return AVERROR(ENOMEM);
    st->codec->codec_type     = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_id       = AV_CODEC_ID_FFWAVESYNTH;
    st->codec->channels       = 2;
    st->codec->channel_layout = AV_CH_LAYOUT_STEREO;
    st->codec->sample_rate    = sbg->sample_rate;
    st->codec->frame_size     = sbg->frame_size;
    avpriv_set_pts_info(st, 64, 1, st->codec->sample_rate);
    st->probe_packets = 0;
    st->start_time    = av_rescale(script.start_ts,
                                   sbg->sample_rate, AV_TIME_BASE);
    st->duration      = script.end_ts == AV_NOPTS_VALUE ? AV_NOPTS_VALUE :
                        av_rescale(script.end_ts - script.start_ts,
                                   sbg->sample_rate, AV_TIME_BASE);
    st->cur_dts       = st->start_time;
    r = encode_intervals(&script, st->codec, &inter);
    if (r < 0)
        goto fail;
    av_free(inter.inter);
    free_script(&script);
    return 0;
fail:
    av_free(inter.inter);
    free_script(&script);
    av_free(buf);
    return r;
}
