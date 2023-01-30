 */
int intf_Create( playlist_t *playlist, const char *chain )
{
    /* Allocate structure */
    intf_thread_t *p_intf = vlc_custom_create( playlist, sizeof( *p_intf ),
                                               "interface" );
    if( unlikely(p_intf == NULL) )
        return VLC_ENOMEM;
    /* Variable used for interface spawning */
    vlc_value_t val, text;
    var_Create( p_intf, "intf-add", VLC_VAR_STRING |
                VLC_VAR_HASCHOICE | VLC_VAR_ISCOMMAND );
    text.psz_string = _("Add Interface");
    var_Change( p_intf, "intf-add", VLC_VAR_SETTEXT, &text, NULL );
#if !defined(_WIN32) && defined(HAVE_ISATTY)
    if( isatty( 0 ) )
#endif
    {
        val.psz_string = (char *)"rc,none";
        text.psz_string = (char *)_("Console");
        var_Change( p_intf, "intf-add", VLC_VAR_ADDCHOICE, &val, &text );
    }
    val.psz_string = (char *)"telnet,none";
    text.psz_string = (char *)_("Telnet");
    var_Change( p_intf, "intf-add", VLC_VAR_ADDCHOICE, &val, &text );
    val.psz_string = (char *)"http,none";
    text.psz_string = (char *)_("Web");
    var_Change( p_intf, "intf-add", VLC_VAR_ADDCHOICE, &val, &text );
    val.psz_string = (char *)"logger,none";
    text.psz_string = (char *)_("Debug logging");
    var_Change( p_intf, "intf-add", VLC_VAR_ADDCHOICE, &val, &text );
    val.psz_string = (char *)"gestures,none";
    text.psz_string = (char *)_("Mouse Gestures");
    var_Change( p_intf, "intf-add", VLC_VAR_ADDCHOICE, &val, &text );
    var_AddCallback( p_intf, "intf-add", AddIntfCallback, playlist );
    /* Choose the best module */
    char *module;
    p_intf->p_cfg = NULL;
    free( config_ChainCreate( &module, &p_intf->p_cfg, chain ) );
    p_intf->p_module = module_need( p_intf, "interface", module, true );
    free(module);
    if( p_intf->p_module == NULL )
    {
        msg_Err( p_intf, "no suitable interface module" );
        goto error;
    }
    vlc_mutex_lock( &lock );
    p_intf->p_next = pl_priv( playlist )->interface;
    pl_priv( playlist )->interface = p_intf;
    vlc_mutex_unlock( &lock );
    return VLC_SUCCESS;
error:
    if( p_intf->p_module )
        module_unneed( p_intf, p_intf->p_module );
    config_ChainDestroy( p_intf->p_cfg );
    vlc_object_release( p_intf );
    return VLC_EGENERIC;
}
