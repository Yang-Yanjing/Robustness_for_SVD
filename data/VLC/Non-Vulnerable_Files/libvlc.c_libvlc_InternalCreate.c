 */
libvlc_int_t * libvlc_InternalCreate( void )
{
    libvlc_int_t *p_libvlc;
    libvlc_priv_t *priv;
    /* Allocate a libvlc instance object */
    p_libvlc = vlc_custom_create( (vlc_object_t *)NULL, sizeof (*priv),
                                  "libvlc" );
    if( p_libvlc == NULL )
        return NULL;
    priv = libvlc_priv (p_libvlc);
    priv->playlist = NULL;
    priv->p_dialog_provider = NULL;
    priv->p_vlm = NULL;
    vlc_ExitInit( &priv->exit );
    return p_libvlc;
}
