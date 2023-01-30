 */
static void vlc_object_destroy( vlc_object_t *p_this )
{
    vlc_object_internals_t *p_priv = vlc_internals( p_this );
    /* Call the custom "subclass" destructor */
    if( p_priv->pf_destructor )
        p_priv->pf_destructor( p_this );
    if (unlikely(p_this == VLC_OBJECT(p_this->p_libvlc)))
    {
        /* TODO: should be in src/libvlc.c */
        var_DelCallback (p_this, "tree", DumpCommand, p_this);
        var_DelCallback (p_this, "vars", DumpCommand, p_this);
    }
    /* Destroy the associated variables. */
    var_DestroyAll( p_this );
    vlc_cond_destroy( &p_priv->var_wait );
    vlc_mutex_destroy( &p_priv->var_lock );
    free( p_this->psz_header );
    free( p_priv->psz_name );
    if( p_priv->pipes[1] != -1 && p_priv->pipes[1] != p_priv->pipes[0] )
        close( p_priv->pipes[1] );
    if( p_priv->pipes[0] != -1 )
        close( p_priv->pipes[0] );
    if( VLC_OBJECT(p_this->p_libvlc) == p_this )
        vlc_mutex_destroy (&(libvlc_priv ((libvlc_int_t *)p_this)->structure_lock));
    free( p_priv );
}
