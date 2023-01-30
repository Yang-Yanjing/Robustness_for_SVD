static void show_subtitle(WriterContext *w, AVSubtitle *sub, AVStream *stream,
                          AVFormatContext *fmt_ctx)
{
    AVBPrint pbuf;
    av_bprint_init(&pbuf, 1, AV_BPRINT_SIZE_UNLIMITED);
    writer_print_section_header(w, SECTION_ID_SUBTITLE);
    print_str ("media_type",         "subtitle");
    print_ts  ("pts",                 sub->pts);
    print_time("pts_time",            sub->pts, &AV_TIME_BASE_Q);
    print_int ("format",              sub->format);
    print_int ("start_display_time",  sub->start_display_time);
    print_int ("end_display_time",    sub->end_display_time);
    print_int ("num_rects",           sub->num_rects);
    writer_print_section_footer(w);
    av_bprint_finalize(&pbuf, NULL);
    fflush(stdout);
}
