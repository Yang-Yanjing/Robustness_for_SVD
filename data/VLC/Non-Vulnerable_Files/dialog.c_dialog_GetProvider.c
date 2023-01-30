}
static vlc_object_t *dialog_GetProvider (vlc_object_t *obj)
{
    libvlc_priv_t *priv = libvlc_priv (obj->p_libvlc);
    vlc_object_t *provider;
    vlc_mutex_lock (&provider_lock);
    if ((provider = priv->p_dialog_provider) != NULL)
        vlc_object_hold (provider);
    vlc_mutex_unlock (&provider_lock);
    return provider;
}
