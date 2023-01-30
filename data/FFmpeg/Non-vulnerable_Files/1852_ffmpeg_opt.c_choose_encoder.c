static int choose_encoder(OptionsContext *o, AVFormatContext *s, OutputStream *ost)
{
    enum AVMediaType type = ost->st->codecpar->codec_type;
    char *codec_name = NULL;
    if (type == AVMEDIA_TYPE_VIDEO || type == AVMEDIA_TYPE_AUDIO || type == AVMEDIA_TYPE_SUBTITLE) {
        MATCH_PER_STREAM_OPT(codec_names, str, codec_name, s, ost->st);
        if (!codec_name) {
            ost->st->codecpar->codec_id = av_guess_codec(s->oformat, NULL, s->filename,
                                                         NULL, ost->st->codecpar->codec_type);
            ost->enc = avcodec_find_encoder(ost->st->codecpar->codec_id);
            if (!ost->enc) {
                av_log(NULL, AV_LOG_FATAL, "Automatic encoder selection failed for "
                       "output stream #%d:%d. Default encoder for format %s (codec %s) is "
                       "probably disabled. Please choose an encoder manually.\n",
                       ost->file_index, ost->index, s->oformat->name,
                       avcodec_get_name(ost->st->codecpar->codec_id));
                return AVERROR_ENCODER_NOT_FOUND;
            }
        } else if (!strcmp(codec_name, "copy"))
            ost->stream_copy = 1;
        else {
            ost->enc = find_codec_or_die(codec_name, ost->st->codecpar->codec_type, 1);
            ost->st->codecpar->codec_id = ost->enc->id;
        }
        ost->encoding_needed = !ost->stream_copy;
    } else {
        
        ost->stream_copy     = 1;
        ost->encoding_needed = 0;
    }
    return 0;
}
