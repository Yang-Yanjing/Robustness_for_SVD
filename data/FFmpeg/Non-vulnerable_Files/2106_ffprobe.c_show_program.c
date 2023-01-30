static int show_program(WriterContext *w, InputFile *ifile, AVProgram *program)
{
    AVFormatContext *fmt_ctx = ifile->fmt_ctx;
    int i, ret = 0;
    writer_print_section_header(w, SECTION_ID_PROGRAM);
    print_int("program_id", program->id);
    print_int("program_num", program->program_num);
    print_int("nb_streams", program->nb_stream_indexes);
    print_int("pmt_pid", program->pmt_pid);
    print_int("pcr_pid", program->pcr_pid);
    print_ts("start_pts", program->start_time);
    print_time("start_time", program->start_time, &AV_TIME_BASE_Q);
    print_ts("end_pts", program->end_time);
    print_time("end_time", program->end_time, &AV_TIME_BASE_Q);
    if (do_show_program_tags)
        ret = show_tags(w, program->metadata, SECTION_ID_PROGRAM_TAGS);
    if (ret < 0)
        goto end;
    writer_print_section_header(w, SECTION_ID_PROGRAM_STREAMS);
    for (i = 0; i < program->nb_stream_indexes; i++) {
        if (selected_streams[program->stream_index[i]]) {
            ret = show_stream(w, fmt_ctx, program->stream_index[i], &ifile->streams[program->stream_index[i]], 1);
            if (ret < 0)
                break;
        }
    }
    writer_print_section_footer(w);
end:
    writer_print_section_footer(w);
    return ret;
}
