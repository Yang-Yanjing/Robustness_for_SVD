static void print_program_info(int flags, int level)
{
    const char *indent = flags & INDENT? "  " : "";
    av_log(NULL, level, "%s version " FFMPEG_VERSION, program_name);
    if (flags & SHOW_COPYRIGHT)
        av_log(NULL, level, " Copyright (c) %d-%d the FFmpeg developers",
               program_birth_year, CONFIG_THIS_YEAR);
    av_log(NULL, level, "\n");
    av_log(NULL, level, "%sbuilt with %s\n", indent, CC_IDENT);
    av_log(NULL, level, "%sconfiguration: " FFMPEG_CONFIGURATION "\n", indent);
}
