static void print_buildconf(int flags, int level)
{
    const char *indent = flags & INDENT ? "  " : "";
    char str[] = { FFMPEG_CONFIGURATION };
    char *conflist, *remove_tilde, *splitconf;
    
    
    while ((conflist = strstr(str, " --")) != NULL) {
        strncpy(conflist, "~--", 3);
    }
    
    
    while ((remove_tilde = strstr(str, "pkg-config~")) != NULL) {
        strncpy(remove_tilde, "pkg-config ", 11);
    }
    splitconf = strtok(str, "~");
    av_log(NULL, level, "\n%sconfiguration:\n", indent);
    while (splitconf != NULL) {
        av_log(NULL, level, "%s%s%s\n", indent, indent, splitconf);
        splitconf = strtok(NULL, "~");
    }
}
