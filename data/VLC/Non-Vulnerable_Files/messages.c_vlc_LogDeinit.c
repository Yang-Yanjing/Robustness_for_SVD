}
void vlc_LogDeinit (libvlc_int_t *vlc)
{
    libvlc_priv_t *priv = libvlc_priv (vlc);
    vlc_rwlock_destroy (&priv->log.lock);
}
