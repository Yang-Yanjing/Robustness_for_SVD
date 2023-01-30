 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    intf_thread_t   *p_intf = (intf_thread_t *)p_this;
    intf_sys_t      *p_sys  = malloc( sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    if( !notify_init( APPLICATION_NAME ) )
    {
        free( p_sys );
        msg_Err( p_intf, "can't find notification daemon" );
        return VLC_EGENERIC;
    }
    p_intf->p_sys = p_sys;
    vlc_mutex_init( &p_sys->lock );
    p_sys->notification = NULL;
    p_sys->b_has_actions = false;
    GList *p_caps = notify_get_server_caps ();
    if( p_caps )
    {
        for( GList *c = p_caps; c != NULL; c = c->next )
        {
            if( !strcmp( (char*)c->data, "actions" ) )
            {
                p_sys->b_has_actions = true;
                break;
            }
        }
        g_list_foreach( p_caps, (GFunc)g_free, NULL );
        g_list_free( p_caps );
    }
    /* */
    var_AddCallback( pl_Get( p_intf ), "activity", ItemChange, p_intf );
    return VLC_SUCCESS;
}
