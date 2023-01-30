 */
int dialog_Register (vlc_object_t *obj)
{
    libvlc_priv_t *priv = libvlc_priv (obj->p_libvlc);
    int ret = VLC_EGENERIC;
    vlc_mutex_lock (&provider_lock);
    if (priv->p_dialog_provider == NULL)
    {   /* Since the object is responsible for unregistering itself before
         * it terminates, at reference is not needed. */
        priv->p_dialog_provider = obj;
        ret = VLC_SUCCESS;
    }
    vlc_mutex_unlock (&provider_lock);
    return ret;
}
