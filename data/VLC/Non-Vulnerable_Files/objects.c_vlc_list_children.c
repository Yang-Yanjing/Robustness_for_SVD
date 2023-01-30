 */
vlc_list_t *vlc_list_children( vlc_object_t *obj )
{
    vlc_list_t *l;
    vlc_object_internals_t *priv;
    unsigned count = 0;
    libvlc_lock (obj->p_libvlc);
    for (priv = vlc_internals (obj)->first; priv; priv = priv->next)
         count++;
    l = NewList (count);
    if (likely(l != NULL))
    {
        unsigned i = 0;
        for (priv = vlc_internals (obj)->first; priv; priv = priv->next)
            l->p_values[i++].p_object = vlc_object_hold (vlc_externals (priv));
    }
    libvlc_unlock (obj->p_libvlc);
    return l;
}
