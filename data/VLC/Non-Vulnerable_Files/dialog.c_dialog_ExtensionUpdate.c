#undef dialog_ExtensionUpdate
int dialog_ExtensionUpdate (vlc_object_t *obj, extension_dialog_t *dialog)
{
    assert (obj);
    assert (dialog);
    vlc_object_t *dp = dialog_GetProvider(obj);
    if (!dp)
    {
        msg_Warn (obj, "Dialog provider is not set, can't update dialog '%s'",
                  dialog->psz_title);
        return VLC_EGENERIC;
    }
    // Signaling the dialog provider
    int ret = var_SetAddress (dp, "dialog-extension", dialog);
    vlc_object_release (dp);
    return ret;
}
