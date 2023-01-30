#undef vlc_custom_create
void *vlc_custom_create (vlc_object_t *parent, size_t length,
                         const char *typename)
{
    /* NOTE:
     * VLC objects are laid out as follow:
     * - first the LibVLC-private per-object data,
     * - then VLC_COMMON members from vlc_object_t,
     * - finally, the type-specific data (if any).
     *
     * This function initializes the LibVLC and common data,
     * and zeroes the rest.
     */
    assert (length >= sizeof (vlc_object_t));
    vlc_object_internals_t *priv = malloc (sizeof (*priv) + length);
    if (unlikely(priv == NULL))
        return NULL;
    priv->psz_name = NULL;
    priv->var_root = NULL;
    vlc_mutex_init (&priv->var_lock);
    vlc_cond_init (&priv->var_wait);
    priv->pipes[0] = priv->pipes[1] = -1;
    atomic_init (&priv->refs, 1);
    priv->pf_destructor = NULL;
    priv->prev = NULL;
    priv->first = NULL;
    vlc_object_t *obj = (vlc_object_t *)(priv + 1);
    obj->psz_object_type = typename;
    obj->psz_header = NULL;
    obj->b_force = false;
    memset (obj + 1, 0, length - sizeof (*obj)); /* type-specific stuff */
    if (likely(parent != NULL))
    {
        vlc_object_internals_t *papriv = vlc_internals (parent);
        obj->i_flags = parent->i_flags;
        obj->p_libvlc = parent->p_libvlc;
        /* Attach the child to its parent (no lock needed) */
        obj->p_parent = vlc_object_hold (parent);
        /* Attach the parent to its child (structure lock needed) */
        libvlc_lock (obj->p_libvlc);
        atomic_init (&priv->alive, atomic_load (&papriv->alive));
        priv->next = papriv->first;
        if (priv->next != NULL)
            priv->next->prev = priv;
        papriv->first = priv;
        libvlc_unlock (obj->p_libvlc);
    }
    else
    {
        libvlc_int_t *self = (libvlc_int_t *)obj;
        obj->i_flags = 0;
        obj->p_libvlc = self;
        obj->p_parent = NULL;
        atomic_init (&priv->alive, true);
        priv->next = NULL;
        vlc_mutex_init (&(libvlc_priv (self)->structure_lock));
        /* TODO: should be in src/libvlc.c */
        int canc = vlc_savecancel ();
        var_Create (obj, "tree", VLC_VAR_STRING | VLC_VAR_ISCOMMAND);
        var_AddCallback (obj, "tree", DumpCommand, obj);
        var_Create (obj, "vars", VLC_VAR_STRING | VLC_VAR_ISCOMMAND);
        var_AddCallback (obj, "vars", DumpCommand, obj);
        vlc_restorecancel (canc);
    }
    return obj;
}
