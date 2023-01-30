#ifdef __ANDROID__
static void AndroidPrintMsg (void *d, int type, const vlc_log_t *p_item,
                             const char *format, va_list ap)
{
    int verbose = (intptr_t)d;
    int prio;
    if (verbose < 0 || verbose < (type - VLC_MSG_ERR))
        return;
    int canc = vlc_savecancel ();
    char *format2;
    if (asprintf (&format2, "[%0*"PRIxPTR"] %s %s: %s",
                  ptr_width, p_item->i_object_id, p_item->psz_module,
                  p_item->psz_object_type, format) < 0)
        return;
    switch (type) {
        case VLC_MSG_INFO:
            prio = ANDROID_LOG_INFO;
            break;
        case VLC_MSG_ERR:
            prio = ANDROID_LOG_ERROR;
            break;
        case VLC_MSG_WARN:
            prio = ANDROID_LOG_WARN;
            break;
        default:
        case VLC_MSG_DBG:
            prio = ANDROID_LOG_DEBUG;
    }
    __android_log_vprint (prio, "VLC", format2, ap);
    free (format2);
    vlc_restorecancel (canc);
}
