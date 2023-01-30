#ifdef _WIN32
static void Win32DebugOutputMsg (void* d, int type, const vlc_log_t *p_item,
                                 const char *format, va_list dol)
{
    VLC_UNUSED(p_item);
    const signed char *pverbose = d;
    if (pverbose && (*pverbose < 0 || *pverbose < (type - VLC_MSG_ERR)))
        return;
    va_list dol2;
    va_copy (dol2, dol);
    int msg_len = vsnprintf(NULL, 0, format, dol2);
    va_end (dol2);
    if(msg_len <= 0)
        return;
    char *msg = malloc(msg_len + 1 + 1);
    if (!msg)
        return;
    msg_len = vsnprintf(msg, msg_len+1, format, dol);
    if (msg_len > 0){
        if(msg[msg_len-1] != '\n'){
            msg[msg_len] = '\n';
            msg[msg_len + 1] = '\0';
        }
        char* psz_msg = NULL;
        if(asprintf(&psz_msg, "%s %s%s: %s", p_item->psz_module,
                    p_item->psz_object_type, msg_type[type], msg) > 0) {
            wchar_t* wmsg = ToWide(psz_msg);
            OutputDebugStringW(wmsg);
            free(wmsg);
            free(psz_msg);
        }
    }
    free(msg);
}
