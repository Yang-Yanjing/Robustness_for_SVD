static int show_programs(WriterContext *w, InputFile *ifile)
{
    AVFormatContext *fmt_ctx = ifile->fmt_ctx;
    int i, ret = 0;
    writer_print_section_header(w, SECTION_ID_PROGRAMS);
    for (i = 0; i < fmt_ctx->nb_programs; i++) {
        AVProgram *program = fmt_ctx->programs[i];
        if (!program)
            continue;
        ret = show_program(w, ifile, program);
        if (ret < 0)
            break;
    }
    writer_print_section_footer(w);
    return ret;
}
