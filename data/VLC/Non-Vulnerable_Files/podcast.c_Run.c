VLC_NORETURN
static void *Run( void *data )
{
    services_discovery_t *p_sd = data;
    services_discovery_sys_t *p_sys  = p_sd->p_sys;
    vlc_mutex_lock( &p_sys->lock );
    mutex_cleanup_push( &p_sys->lock );
    for( ;; )
    {
        while( !p_sys->b_update )
            vlc_cond_wait( &p_sys->wait, &p_sys->lock );
        int canc = vlc_savecancel ();
        msg_Dbg( p_sd, "Update required" );
        if( p_sys->update_type == UPDATE_URLS )
        {
            char *psz_urls = var_GetNonEmptyString( p_sd->p_parent,
                                                    "podcast-urls" );
            ParseUrls( p_sd, psz_urls );
            free( psz_urls );
        }
        else if( p_sys->update_type == UPDATE_REQUEST )
            ParseRequest( p_sd );
        p_sys->b_update = false;
        for( int i = 0; i < p_sd->p_sys->i_input; i++ )
        {
            input_thread_t *p_input = p_sd->p_sys->pp_input[i];
            if( p_input->b_eof || p_input->b_error )
            {
                input_Stop( p_input, false );
                input_Close( p_input );
                p_sd->p_sys->pp_input[i] = NULL;
                REMOVE_ELEM( p_sys->pp_input, p_sys->i_input, i );
                i--;
            }
        }
        vlc_restorecancel (canc);
    }
    vlc_cleanup_pop();
    assert(0); /* dead code */
}
