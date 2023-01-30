 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    /* Stop vout event handler */
    var_DelCallback( p_sys->p_input, "intf-event", EventIntf, p_demux );
    if( p_sys->p_vout != NULL )
    {   /* Should not happen, but better be safe than sorry. */
        msg_Warn( p_sys->p_vout, "removing dangling mouse DVD callbacks" );
        var_DelCallback( p_sys->p_vout, "mouse-moved", EventMouse, p_demux );
        var_DelCallback( p_sys->p_vout, "mouse-clicked", EventMouse, p_demux );
    }
    /* Stop still image handler */
    if( p_sys->still.b_created )
        vlc_timer_destroy( p_sys->still.timer );
    vlc_mutex_destroy( &p_sys->still.lock );
    var_Destroy( p_sys->p_input, "highlight" );
    var_Destroy( p_sys->p_input, "x-start" );
    var_Destroy( p_sys->p_input, "x-end" );
    var_Destroy( p_sys->p_input, "y-start" );
    var_Destroy( p_sys->p_input, "y-end" );
    var_Destroy( p_sys->p_input, "color" );
    var_Destroy( p_sys->p_input, "menu-palette" );
    vlc_object_release( p_sys->p_input );
    for( int i = 0; i < PS_TK_COUNT; i++ )
    {
        ps_track_t *tk = &p_sys->tk[i];
        if( tk->b_seen )
        {
            es_format_Clean( &tk->fmt );
            if( tk->es ) es_out_Del( p_demux->out, tk->es );
        }
    }
    /* Free the array of titles */
    for( int i = 0; i < p_sys->i_title; i++ )
        vlc_input_title_Delete( p_sys->title[i] );
    TAB_CLEAN( p_sys->i_title, p_sys->title );
    dvdnav_close( p_sys->dvdnav );
    free( p_sys );
}
