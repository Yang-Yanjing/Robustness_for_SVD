dialog_progress_bar_t *
dialog_ProgressCreate (vlc_object_t *obj, const char *title,
                       const char *message, const char *cancel)
{
    if (obj->i_flags & OBJECT_FLAGS_NOINTERACT)
        return NULL;
    vlc_object_t *provider = dialog_GetProvider (obj);
    if (provider == NULL)
        return NULL;
    dialog_progress_bar_t *dialog = malloc (sizeof (*dialog));
    if (dialog != NULL)
    {
        dialog->title = title;
        dialog->message = message;
        dialog->cancel = cancel;
        var_SetAddress (provider, "dialog-progress-bar", dialog);
#ifndef NDEBUG
        dialog->title = dialog->message = dialog->cancel = NULL;
#endif
        assert (dialog->pf_update);
        assert (dialog->pf_check);
        assert (dialog->pf_destroy);
    }
    /* FIXME: This could conceivably crash if the dialog provider is destroyed
     * before the dialog user. Holding the provider does not help, as it only
     * protects object variable operations. For instance, it does not prevent
     * unloading of the interface plugin. In the short term, the only solution
     * is to not use progress dialog after deinitialization of the interfaces.
     */
    vlc_object_release (provider);
    return dialog;
}
