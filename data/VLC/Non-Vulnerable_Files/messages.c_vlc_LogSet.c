 */
void vlc_LogSet (libvlc_int_t *vlc, vlc_log_cb cb, void *opaque)
{
    libvlc_priv_t *priv = libvlc_priv (vlc);
    if (cb == NULL)
    {
#ifdef __ANDROID__
        cb = AndroidPrintMsg;
#else
#if defined (HAVE_ISATTY) && !defined (_WIN32)
        if (isatty (STDERR_FILENO) && var_InheritBool (vlc, "color"))
            cb = PrintColorMsg;
        else
#endif
            cb = PrintMsg;
#endif // __ANDROID__
        opaque = (void *)(intptr_t)priv->log.verbose;
    }
    vlc_rwlock_wrlock (&priv->log.lock);
    priv->log.cb = cb;
    priv->log.opaque = opaque;
    vlc_rwlock_unlock (&priv->log.lock);
    /* Announce who we are */
    msg_Dbg (vlc, "VLC media player - %s", VERSION_MESSAGE);
    msg_Dbg (vlc, "%s", COPYRIGHT_MESSAGE);
    msg_Dbg (vlc, "revision %s", psz_vlc_changeset);
    msg_Dbg (vlc, "configured with %s", CONFIGURE_LINE);
}
