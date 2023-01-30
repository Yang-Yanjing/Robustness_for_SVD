 */
int libvlc_InternalAddIntf(libvlc_int_t *libvlc, const char *name)
{
    playlist_t *playlist = intf_GetPlaylist(libvlc);
    int ret;
    if (unlikely(playlist == NULL))
        ret = VLC_ENOMEM;
    else
    if (name != NULL)
        ret = intf_Create(playlist, name);
    else
    {   /* Default interface */
        char *intf = var_InheritString(libvlc, "intf");
        if (intf == NULL) /* "intf" has not been set */
        {
#if !defined(_WIN32) && !defined(__OS2__)
            char *pidfile = var_InheritString(libvlc, "pidfile");
            if (pidfile != NULL)
                free(pidfile);
            else
#endif
                msg_Info(libvlc, _("Running vlc with the default interface. "
                         "Use 'cvlc' to use vlc without interface."));
        }
        ret = intf_Create(playlist, intf);
        free(intf);
        name = "default";
    }
    if (ret != VLC_SUCCESS)
        msg_Err(libvlc, "interface \"%s\" initialization failed", name);
    return ret;
}
