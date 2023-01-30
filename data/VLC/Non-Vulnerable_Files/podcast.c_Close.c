 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    services_discovery_t *p_sd = ( services_discovery_t* )p_this;
    services_discovery_sys_t *p_sys = p_sd->p_sys;
    vlc_object_t *pl = p_sd->p_parent;
    int i;
    vlc_cancel (p_sys->thread);
    vlc_join (p_sys->thread, NULL);
    var_DelCallback( pl, "podcast-urls", UrlsChange, p_sys );
    var_DelCallback( pl, "podcast-request", Request, p_sys );
    vlc_cond_destroy( &p_sys->wait );
    vlc_mutex_destroy( &p_sys->lock );
    for( i = 0; i < p_sys->i_input; i++ )
    {
        input_thread_t *p_input = p_sd->p_sys->pp_input[i];
        if( !p_input )
            continue;
        input_Stop( p_input, true );
        input_Close( p_input );
        p_sd->p_sys->pp_input[i] = NULL;
    }
    free( p_sd->p_sys->pp_input );
    for( i = 0; i < p_sys->i_urls; i++ ) free( p_sys->ppsz_urls[i] );
    free( p_sys->ppsz_urls );
    for( i = 0; i < p_sys->i_items; i++ ) vlc_gc_decref( p_sys->pp_items[i] );
    free( p_sys->pp_items );
    free( p_sys->psz_request );
    free( p_sys );
}
