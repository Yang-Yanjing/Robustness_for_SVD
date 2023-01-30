 */
int dialog_Unregister (vlc_object_t *obj)
{
    libvlc_priv_t *priv = libvlc_priv (obj->p_libvlc);
    int ret = VLC_EGENERIC;
    vlc_mutex_lock (&provider_lock);
    if (priv->p_dialog_provider == obj)
    {
        priv->p_dialog_provider = NULL;
        ret = VLC_SUCCESS;
    }
    vlc_mutex_unlock (&provider_lock);
    return ret;
}
