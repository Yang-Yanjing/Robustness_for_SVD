static const int ptr_width = 2 * /* hex digits */ sizeof(uintptr_t);
static void PrintColorMsg (void *d, int type, const vlc_log_t *p_item,
                           const char *format, va_list ap)
{
    FILE *stream = stderr;
    int verbose = (intptr_t)d;
    if (verbose < 0 || verbose < (type - VLC_MSG_ERR))
        return;
    int canc = vlc_savecancel ();
    flockfile (stream);
    utf8_fprintf (stream, "["GREEN"%0*"PRIxPTR""GRAY"] ", ptr_width, p_item->i_object_id);
    if (p_item->psz_header != NULL)
        utf8_fprintf (stream, "[%s] ", p_item->psz_header);
    utf8_fprintf (stream, "%s %s%s: %s", p_item->psz_module,
                  p_item->psz_object_type, msg_type[type], msg_color[type]);
    utf8_vfprintf (stream, format, ap);
    fputs (GRAY"\n", stream);
#if defined (_WIN32) || defined (__OS2__)
    fflush (stream);
#endif
    funlockfile (stream);
    vlc_restorecancel (canc);
}
