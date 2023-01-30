}
void vlc_LogInit (libvlc_int_t *vlc)
{
    libvlc_priv_t *priv = libvlc_priv (vlc);
    const char *str;
    if (var_InheritBool (vlc, "quiet"))
        priv->log.verbose = -1;
    else
    if ((str = getenv ("VLC_VERBOSE")) != NULL)
        priv->log.verbose = atoi (str);
    else
        priv->log.verbose = var_InheritInteger (vlc, "verbose");
    vlc_rwlock_init (&priv->log.lock);
    vlc_LogSet (vlc, NULL, NULL);
}
