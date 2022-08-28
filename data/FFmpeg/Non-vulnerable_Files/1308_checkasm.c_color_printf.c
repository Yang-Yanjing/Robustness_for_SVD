static void color_printf(int color, const char *fmt, ...)
{
    static int use_color = -1;
    va_list arg;
#if HAVE_SETCONSOLETEXTATTRIBUTE
    static HANDLE con;
    static WORD org_attributes;
    if (use_color < 0) {
        CONSOLE_SCREEN_BUFFER_INFO con_info;
        con = GetStdHandle(STD_ERROR_HANDLE);
        if (con && con != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(con, &con_info)) {
            org_attributes = con_info.wAttributes;
            use_color = 1;
        } else
            use_color = 0;
    }
    if (use_color)
        SetConsoleTextAttribute(con, (org_attributes & 0xfff0) | (color & 0x0f));
#else
    if (use_color < 0) {
        const char *term = getenv("TERM");
        use_color = term && strcmp(term, "dumb") && isatty(2);
    }
    if (use_color)
        fprintf(stderr, "\x1b[%d;3%dm", (color & 0x08) >> 3, color & 0x07);
#endif
    va_start(arg, fmt);
    vfprintf(stderr, fmt, arg);
    va_end(arg);
    if (use_color) {
#if HAVE_SETCONSOLETEXTATTRIBUTE
        SetConsoleTextAttribute(con, org_attributes);
#else
        fprintf(stderr, "\x1b[0m");
#endif
    }
}
