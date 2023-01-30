#undef vlc_object_get_name
char *vlc_object_get_name(const vlc_object_t *obj)
{
    vlc_object_internals_t *priv = vlc_internals(obj);
    char *name;
    vlc_mutex_lock (&name_lock);
    name = priv->psz_name ? strdup (priv->psz_name) : NULL;
    vlc_mutex_unlock (&name_lock);
    return name;
}
