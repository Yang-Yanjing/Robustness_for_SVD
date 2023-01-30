 */
void libvlc_InternalCleanup( libvlc_int_t *p_libvlc )
{
    libvlc_priv_t *priv = libvlc_priv (p_libvlc);
    /* Ask the interfaces to stop and destroy them */
    msg_Dbg( p_libvlc, "removing all interfaces" );
    libvlc_Quit( p_libvlc );
    intf_DestroyAll( p_libvlc );
#ifdef ENABLE_VLM
    /* Destroy VLM if created in libvlc_InternalInit */
    if( priv->p_vlm )
    {
        vlm_Delete( priv->p_vlm );
    }
#endif
#if !defined( _WIN32 ) && !defined( __OS2__ )
    char *pidfile = var_InheritString( p_libvlc, "pidfile" );
    if( pidfile != NULL )
    {
        msg_Dbg( p_libvlc, "removing PID file %s", pidfile );
        if( unlink( pidfile ) )
            msg_Warn( p_libvlc, "cannot remove PID file %s: %s",
                      pidfile, vlc_strerror_c(errno) );
        free( pidfile );
    }
#endif
    if (priv->parser != NULL)
        playlist_preparser_Delete(priv->parser);
    vlc_DeinitActions( p_libvlc, priv->actions );
    /* Save the configuration */
    if( !var_InheritBool( p_libvlc, "ignore-config" ) )
        config_AutoSaveConfigFile( VLC_OBJECT(p_libvlc) );
    /* Free module bank. It is refcounted, so we call this each time  */
    module_EndBank (true);
    vlc_LogDeinit (p_libvlc);
#if defined(_WIN32) || defined(__OS2__)
    system_End( );
#endif
}
