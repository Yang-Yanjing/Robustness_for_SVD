static int opt_map_channel(void *optctx, const char *opt, const char *arg)
{
    OptionsContext *o = optctx;
    int n;
    AVStream *st;
    AudioChannelMap *m;
    GROW_ARRAY(o->audio_channel_maps, o->nb_audio_channel_maps);
    m = &o->audio_channel_maps[o->nb_audio_channel_maps - 1];
    
    n = sscanf(arg, "%d:%d.%d", &m->channel_idx, &m->ofile_idx, &m->ostream_idx);
    if ((n == 1 || n == 3) && m->channel_idx == -1) {
        m->file_idx = m->stream_idx = -1;
        if (n == 1)
            m->ofile_idx = m->ostream_idx = -1;
        return 0;
    }
    
    n = sscanf(arg, "%d.%d.%d:%d.%d",
               &m->file_idx,  &m->stream_idx, &m->channel_idx,
               &m->ofile_idx, &m->ostream_idx);
    if (n != 3 && n != 5) {
        av_log(NULL, AV_LOG_FATAL, "Syntax error, mapchan usage: "
               "[file.stream.channel|-1][:syncfile:syncstream]\n");
        exit_program(1);
    }
    if (n != 5) 
        m->ofile_idx = m->ostream_idx = -1;
    
    if (m->file_idx < 0 || m->file_idx >= nb_input_files) {
        av_log(NULL, AV_LOG_FATAL, "mapchan: invalid input file index: %d\n",
               m->file_idx);
        exit_program(1);
    }
    if (m->stream_idx < 0 ||
        m->stream_idx >= input_files[m->file_idx]->nb_streams) {
        av_log(NULL, AV_LOG_FATAL, "mapchan: invalid input file stream index #%d.%d\n",
               m->file_idx, m->stream_idx);
        exit_program(1);
    }
    st = input_files[m->file_idx]->ctx->streams[m->stream_idx];
    if (st->codecpar->codec_type != AVMEDIA_TYPE_AUDIO) {
        av_log(NULL, AV_LOG_FATAL, "mapchan: stream #%d.%d is not an audio stream.\n",
               m->file_idx, m->stream_idx);
        exit_program(1);
    }
    if (m->channel_idx < 0 || m->channel_idx >= st->codecpar->channels) {
        av_log(NULL, AV_LOG_FATAL, "mapchan: invalid audio channel #%d.%d.%d\n",
               m->file_idx, m->stream_idx, m->channel_idx);
        exit_program(1);
    }
    return 0;
}
