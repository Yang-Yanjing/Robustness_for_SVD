 */
void libvlc_InternalDestroy( libvlc_int_t *p_libvlc )
{
    libvlc_priv_t *priv = libvlc_priv( p_libvlc );
    vlc_ExitDestroy( &priv->exit );
    assert( atomic_load(&(vlc_internals(p_libvlc)->refs)) == 1 );
    vlc_object_release( p_libvlc );
}
