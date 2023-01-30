}
static void PrintObject( vlc_object_internals_t *priv,
                         const char *psz_prefix )
{
    char psz_refcount[20], psz_name[50], psz_parent[20];
    int canc = vlc_savecancel ();
    memset( &psz_name, 0, sizeof(psz_name) );
    vlc_mutex_lock (&name_lock);
    if (priv->psz_name != NULL)
    {
        snprintf( psz_name, 49, " \"%s\"", priv->psz_name );
        if( psz_name[48] )
            psz_name[48] = '\"';
    }
    vlc_mutex_unlock (&name_lock);
    snprintf( psz_refcount, 19, ", %u refs", atomic_load( &priv->refs ) );
    psz_parent[0] = '\0';
    /* FIXME: need structure lock!!! */
    if( vlc_externals(priv)->p_parent )
        snprintf( psz_parent, 19, ", parent %p",
                  vlc_externals(priv)->p_parent );
    printf( " %so %p %s%s%s%s\n", psz_prefix,
            vlc_externals(priv), vlc_externals(priv)->psz_object_type,
            psz_name, psz_refcount, psz_parent );
    vlc_restorecancel (canc);
}
