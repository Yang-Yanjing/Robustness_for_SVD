static OutputStream *new_data_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
    OutputStream *ost;
    ost = new_output_stream(o, oc, AVMEDIA_TYPE_DATA, source_index);
    if (!ost->stream_copy) {
        av_log(NULL, AV_LOG_FATAL, "Data stream encoding not supported yet (only streamcopy)\n");
        exit_program(1);
    }
    return ost;
}
