static void ffprobe_show_program_version(WriterContext *w)
{
    AVBPrint pbuf;
    av_bprint_init(&pbuf, 1, AV_BPRINT_SIZE_UNLIMITED);
    writer_print_section_header(w, SECTION_ID_PROGRAM_VERSION);
    print_str("version", FFMPEG_VERSION);
    print_fmt("copyright", "Copyright (c) %d-%d the FFmpeg developers",
              program_birth_year, CONFIG_THIS_YEAR);
    print_str("compiler_ident", CC_IDENT);
    print_str("configuration", FFMPEG_CONFIGURATION);
    writer_print_section_footer(w);
    av_bprint_finalize(&pbuf, NULL);
}
