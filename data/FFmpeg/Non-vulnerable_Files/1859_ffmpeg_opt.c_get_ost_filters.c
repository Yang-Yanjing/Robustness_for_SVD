static char *get_ost_filters(OptionsContext *o, AVFormatContext *oc,
                             OutputStream *ost)
{
    AVStream *st = ost->st;
    if (ost->filters_script && ost->filters) {
        av_log(NULL, AV_LOG_ERROR, "Both -filter and -filter_script set for "
               "output stream #%d:%d.\n", nb_output_files, st->index);
        exit_program(1);
    }
    if (ost->filters_script)
        return read_file(ost->filters_script);
    else if (ost->filters)
        return av_strdup(ost->filters);
    return av_strdup(st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ?
                     "null" : "anull");
}
