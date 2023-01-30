/* Following functions are local */
static vlc_object_t *FindName (vlc_object_internals_t *priv, const char *name)
{
    if (priv->psz_name != NULL && !strcmp (priv->psz_name, name))
        return vlc_object_hold (vlc_externals (priv));
    for (priv = priv->first; priv != NULL; priv = priv->next)
    {
        vlc_object_t *found = FindName (priv, name);
        if (found != NULL)
            return found;
    }
    return NULL;
}
