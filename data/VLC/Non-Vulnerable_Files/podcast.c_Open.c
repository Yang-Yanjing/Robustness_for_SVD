 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    if( strcmp( p_this->p_parent->psz_object_type, "playlist" ) )
        return VLC_EGENERIC; /* FIXME: support LibVLC SD too! */
    services_discovery_t *p_sd = ( services_discovery_t* )p_this;
    services_discovery_sys_t *p_sys = malloc( sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->i_urls = 0;
    p_sys->ppsz_urls = NULL;
    p_sys->i_input = 0;
    p_sys->pp_input = NULL;
    p_sys->pp_items = NULL;
    p_sys->i_items = 0;
    vlc_mutex_init( &p_sys->lock );
    vlc_cond_init( &p_sys->wait );
    p_sys->b_update = true;
    p_sys->b_savedurls_loaded = false;
    p_sys->psz_request = NULL;
    p_sys->update_type = UPDATE_URLS;
    p_sd->p_sys  = p_sys;
    /* Launch the callback associated with this variable */
    vlc_object_t *pl = p_sd->p_parent;
    var_Create( pl, "podcast-urls", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    var_AddCallback( pl, "podcast-urls", UrlsChange, p_sys );
    var_Create( pl, "podcast-request", VLC_VAR_STRING );
    var_AddCallback( pl, "podcast-request", Request, p_sys );
    if (vlc_clone (&p_sys->thread, Run, p_sd, VLC_THREAD_PRIORITY_LOW))
    {
        var_DelCallback( pl, "podcast-request", Request, p_sys );
        var_DelCallback( pl, "podcast-urls", UrlsChange, p_sys );
        vlc_cond_destroy( &p_sys->wait );
        vlc_mutex_destroy( &p_sys->lock );
        free (p_sys);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
