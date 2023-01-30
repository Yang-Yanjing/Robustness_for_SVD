 */
void dialog_Login (vlc_object_t *obj, char **username, char **password,
                   const char *title, const char *fmt, ...)
{
    assert ((username != NULL) && (password != NULL));
    *username = *password = NULL;
    if (obj->i_flags & OBJECT_FLAGS_NOINTERACT)
        return;
    vlc_object_t *provider = dialog_GetProvider (obj);
    if (provider == NULL)
        return;
    char *text;
    va_list ap;
    va_start (ap, fmt);
    if (vasprintf (&text, fmt, ap) != -1)
    {
        dialog_login_t dialog = { title, text, username, password, };
        var_SetAddress (provider, "dialog-login", &dialog);
        free (text);
    }
    va_end (ap);
    vlc_object_release (provider);
}
