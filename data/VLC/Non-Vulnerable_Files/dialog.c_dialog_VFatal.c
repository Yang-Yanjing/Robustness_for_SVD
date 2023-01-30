 */
void dialog_VFatal (vlc_object_t *obj, bool modal, const char *title,
                    const char *fmt, va_list ap)
{
    char *text;
    if (obj->i_flags & OBJECT_FLAGS_NOINTERACT)
        return;
    vlc_object_t *provider = dialog_GetProvider (obj);
    if (provider == NULL)
    {
        msg_Err (obj, "%s", title);
        msg_GenericVa (obj, VLC_MSG_ERR, MODULE_STRING, fmt, ap);
        return;
    }
    if (vasprintf (&text, fmt, ap) != -1)
    {
        dialog_fatal_t dialog = { title, text, };
        var_SetAddress (provider,
                        modal ? "dialog-critical" : "dialog-error", &dialog);
        free (text);
    }
    vlc_object_release (provider);
}
