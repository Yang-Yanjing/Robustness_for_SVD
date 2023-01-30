static void show_error(WriterContext *w, int err)
{
    char errbuf[128];
    const char *errbuf_ptr = errbuf;
    if (av_strerror(err, errbuf, sizeof(errbuf)) < 0)
        errbuf_ptr = strerror(AVUNERROR(err));
    writer_print_section_header(w, SECTION_ID_ERROR);
    print_int("code", err);
    print_str("string", errbuf_ptr);
    writer_print_section_footer(w);
}
