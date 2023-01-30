static OutputStream *new_audio_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
    int n;
    AVStream *st;
    OutputStream *ost;
    AVCodecContext *audio_enc;
    ost = new_output_stream(o, oc, AVMEDIA_TYPE_AUDIO, source_index);
    st  = ost->st;
    audio_enc = ost->enc_ctx;
    audio_enc->codec_type = AVMEDIA_TYPE_AUDIO;
    MATCH_PER_STREAM_OPT(filter_scripts, str, ost->filters_script, oc, st);
    MATCH_PER_STREAM_OPT(filters,        str, ost->filters,        oc, st);
    if (!ost->stream_copy) {
        char *sample_fmt = NULL;
        MATCH_PER_STREAM_OPT(audio_channels, i, audio_enc->channels, oc, st);
        MATCH_PER_STREAM_OPT(sample_fmts, str, sample_fmt, oc, st);
        if (sample_fmt &&
            (audio_enc->sample_fmt = av_get_sample_fmt(sample_fmt)) == AV_SAMPLE_FMT_NONE) {
            av_log(NULL, AV_LOG_FATAL, "Invalid sample format '%s'\n", sample_fmt);
            exit_program(1);
        }
        MATCH_PER_STREAM_OPT(audio_sample_rate, i, audio_enc->sample_rate, oc, st);
        MATCH_PER_STREAM_OPT(apad, str, ost->apad, oc, st);
        ost->apad = av_strdup(ost->apad);
        ost->avfilter = get_ost_filters(o, oc, ost);
        if (!ost->avfilter)
            exit_program(1);
        
        for (n = 0; n < o->nb_audio_channel_maps; n++) {
            AudioChannelMap *map = &o->audio_channel_maps[n];
            if ((map->ofile_idx   == -1 || ost->file_index == map->ofile_idx) &&
                (map->ostream_idx == -1 || ost->st->index  == map->ostream_idx)) {
                InputStream *ist;
                if (map->channel_idx == -1) {
                    ist = NULL;
                } else if (ost->source_index < 0) {
                    av_log(NULL, AV_LOG_FATAL, "Cannot determine input stream for channel mapping %d.%d\n",
                           ost->file_index, ost->st->index);
                    continue;
                } else {
                    ist = input_streams[ost->source_index];
                }
                if (!ist || (ist->file_index == map->file_idx && ist->st->index == map->stream_idx)) {
                    if (av_reallocp_array(&ost->audio_channels_map,
                                          ost->audio_channels_mapped + 1,
                                          sizeof(*ost->audio_channels_map)
                                          ) < 0 )
                        exit_program(1);
                    ost->audio_channels_map[ost->audio_channels_mapped++] = map->channel_idx;
                }
            }
        }
    }
    if (ost->stream_copy)
        check_streamcopy_filters(o, oc, ost, AVMEDIA_TYPE_AUDIO);
    return ost;
}
