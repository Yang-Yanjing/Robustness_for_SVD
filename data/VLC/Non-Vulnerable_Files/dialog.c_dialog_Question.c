 */
int dialog_Question (vlc_object_t *obj, const char *title, const char *fmt,
                     const char *yes, const char *no, const char *cancel, ...)
{
    if (obj->i_flags & OBJECT_FLAGS_NOINTERACT)
        return 0;
    vlc_object_t *provider = dialog_GetProvider (obj);
    if (provider == NULL)
        return 0;
    char *text;
    va_list ap;
    int answer = 0;
    va_start (ap, cancel);
    if (vasprintf (&text, fmt, ap) != -1)
    {
        dialog_question_t dialog = { title, text, yes, no, cancel, 0, };
        var_SetAddress (provider, "dialog-question", &dialog);
        answer = dialog.answer;
    }
    va_end (ap);
    vlc_object_release (provider);
    return answer;
}
