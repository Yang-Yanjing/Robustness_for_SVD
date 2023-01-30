static int show_streams(WriterContext *w, InputFile *ifile)
{
    AVFormatContext *fmt_ctx = ifile->fmt_ctx;
    int i, ret = 0;
    writer_print_section_header(w, SECTION_ID_STREAMS);
    for (i = 0; i < ifile->nb_streams; i++)
        if (selected_streams[i]) {
            ret = show_stream(w, fmt_ctx, i, &ifile->streams[i], 0);
            if (ret < 0)
                break;
        }
    writer_print_section_footer(w);
    return ret;
}
