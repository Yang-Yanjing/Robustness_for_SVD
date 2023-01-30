} inhibit_t;
vlc_inhibit_t *vlc_inhibit_Create (vlc_object_t *parent)
{
    inhibit_t *priv = vlc_custom_create (parent, sizeof (*priv), "inhibit" );
    if (priv == NULL)
        return NULL;
    vlc_inhibit_t *ih = &priv->ih;
    ih->p_sys = NULL;
    ih->inhibit = NULL;
    priv->module = module_need (ih, "inhibit", NULL, false);
    if (priv->module == NULL)
    {
        vlc_object_release (ih);
        ih = NULL;
    }
    return ih;
}
