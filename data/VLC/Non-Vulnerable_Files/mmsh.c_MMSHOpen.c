 ****************************************************************************/
int MMSHOpen( access_t *p_access )
{
    access_sys_t    *p_sys;
    char            *psz_location = NULL;
    char            *psz_proxy;
    STANDARD_BLOCK_ACCESS_INIT
    p_sys->i_proto= MMS_PROTO_HTTP;
    p_sys->fd     = -1;
    /* Handle proxy */
    p_sys->b_proxy = false;
    memset( &p_sys->proxy, 0, sizeof(p_sys->proxy) );
    /* Check proxy */
    /* TODO reuse instead http-proxy from http access ? */
    psz_proxy = var_CreateGetNonEmptyString( p_access, "mmsh-proxy" );
    if( !psz_proxy )
    {
        char *psz_http_proxy = var_InheritString( p_access, "http-proxy" );
        if( psz_http_proxy )
        {
            psz_proxy = psz_http_proxy;
            var_SetString( p_access, "mmsh-proxy", psz_proxy );
        }
    }
    if( psz_proxy )
    {
        p_sys->b_proxy = true;
        vlc_UrlParse( &p_sys->proxy, psz_proxy, 0 );
        free( psz_proxy );
    }
    else
    {
        const char *http_proxy = getenv( "http_proxy" );
        if( http_proxy )
        {
            p_sys->b_proxy = true;
            vlc_UrlParse( &p_sys->proxy, http_proxy, 0 );
        }
    }
    if( p_sys->b_proxy )
    {
        if( ( p_sys->proxy.psz_host == NULL ) ||
            ( *p_sys->proxy.psz_host == '\0' ) )
        {
            msg_Warn( p_access, "invalid proxy host" );
            vlc_UrlClean( &p_sys->proxy );
            free( p_sys );
            return VLC_EGENERIC;
        }
        if( p_sys->proxy.i_port <= 0 )
            p_sys->proxy.i_port = 80;
        msg_Dbg( p_access, "Using http proxy %s:%d",
                 p_sys->proxy.psz_host, p_sys->proxy.i_port );
    }
    /* open a tcp connection */
    vlc_UrlParse( &p_sys->url, p_access->psz_location, 0 );
    if( ( p_sys->url.psz_host == NULL ) ||
        ( *p_sys->url.psz_host == '\0' ) )
    {
        msg_Err( p_access, "invalid host" );
        goto error;
    }
    if( p_sys->url.i_port <= 0 )
        p_sys->url.i_port = 80;
    if( Describe( p_access, &psz_location ) )
        goto error;
    /* Handle redirection */
    if( psz_location && *psz_location )
    {
        msg_Dbg( p_access, "redirection to %s", psz_location );
        input_thread_t * p_input = access_GetParentInput( p_access );
        input_item_t * p_new_loc;
        if( !p_input )
        {
            free( psz_location );
            goto error;
        }
        /** \bug we do not autodelete here */
        p_new_loc = input_item_New( psz_location, psz_location );
        input_item_t *p_item = input_GetItem( p_input );
        input_item_PostSubItem( p_item, p_new_loc );
        vlc_gc_decref( p_new_loc );
        vlc_object_release( p_input );
        free( psz_location );
        p_access->pf_block = NULL;
        p_access->pf_read = ReadRedirect;
        return VLC_SUCCESS;
    }
    free( psz_location );
    /* Start playing */
    if( Start( p_access, 0 ) )
    {
        msg_Err( p_access, "cannot start stream" );
        free( p_sys->p_header );
        goto error;
    }
    return VLC_SUCCESS;
error:
    vlc_UrlClean( &p_sys->proxy );
    vlc_UrlClean( &p_sys->url );
    free( p_sys );
    return VLC_EGENERIC;
}
