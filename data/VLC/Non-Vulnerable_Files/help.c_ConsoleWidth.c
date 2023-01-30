 */
static unsigned ConsoleWidth(void)
{
#ifdef TIOCGWINSZ
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
        return ws.ws_col;
#endif
#ifdef WIOCGETD
    struct uwdata uw;
    if (ioctl(STDOUT_FILENO, WIOCGETD, &uw) == 0)
        return uw.uw_height / uw.uw_vs;
#endif
#if defined (_WIN32) && !VLC_WINSTORE_APP
    CONSOLE_SCREEN_BUFFER_INFO buf;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buf))
        return buf.dwSize.X;
#endif
    return 80;
}
